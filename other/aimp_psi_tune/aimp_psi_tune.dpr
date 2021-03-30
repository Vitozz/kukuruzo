library aimp_psi_tune;
uses
  AIMP_SDK,
  SysUtils,
  Windows,
  PSAPI,
  Forms,
  IniFiles,
  optDialog;
{$R *.res}
type
{ TAIMP2TuneInfo }
  TAIMP2TuneInfo = class(TInterfacedObject, IAIMPAddonHeader)
  protected
    function SetPluginAuthor: PWideChar; stdcall;
    function SetPluginName: PWideChar; stdcall;
    function SetHasSettingsDialog: LongBool; stdcall;
    procedure Initialize(AController: IAIMP2Controller); stdcall;
    procedure Finalize; stdcall;
    procedure ShowSettingsDialog(Wind: HWND); stdcall;
  private
    FAIMPClassic: IAIMP2Controller;
  public
    constructor Create(AIMP: IAIMP2Controller);
    destructor Destroy; override;
    procedure IAIMPAddonHeader.Initialize = Initialize;
    procedure IAIMPAddonHeader.Finalize = Finalize;
    function IAIMPAddonHeader.GetHasSettingsDialog = SetHasSettingsDialog;
    function IAIMPAddonHeader.GetPluginAuthor = SetPluginAuthor;
    function IAIMPAddonHeader.GetPluginName = SetPluginName;
    procedure IAIMPAddonHeader.ShowSettingsDialog = ShowSettingsDialog;
  end;
//constants
const
  PORTABLE_NAME = 'psi-plus-portable.exe';
  PSIDATA = 'Psi+';
  TUNEFILE = '\tune';
  PROFILE = 'USERPROFILE';
  DATADIR = 'PSIPLUSDATADIR';
  XP_SUFFIX = '\Local Settings\Application Data';
  LOCALDATA = 'LOCALAPPDATA';
  CONFFILE = '\aimp_psi_tune.ini';
  INI_MAIN_SECTION = 'Tune';
  PSI_NAME_KEY = 'psi_name';
  ARTIST_KEY = 'arttag';
  TITLE_KEY = 'titltag';
  ALBUM_KEY = 'albtag';
  PSI_NAME = 0;
  PSI_ARTIST = 1;
  PSI_TITLE = 2;
  PSI_ALBUM = 3;
  CLEAR_FILE = 1;
  WRITE_FILE = 0;
  AUTHOR = 'Vitaly Tonkacheyev <thetvg@gmail.com>';
  PLUGNAME = 'Tune To Psi';
  NONE = '';
  BUF_SIZE = 89;
  TAG_SIZE = 90;
  SPACE = ' ';
  SEP = '\';
//
function GetSettings(typeof: ShortInt):WideString; stdcall;
var
  inifile: TIniFile;
  filePath : String;
begin
  filePath := GetCurrentDir();
  inifile:= TIniFile.Create(filePath + CONFFILE);
  if not inifile.SectionExists(INI_MAIN_SECTION) then
  begin
    inifile.WriteString(INI_MAIN_SECTION, PSI_NAME_KEY, PORTABLE_NAME);
    inifile.WriteString(INI_MAIN_SECTION, ARTIST_KEY, SPACE);
    inifile.WriteString(INI_MAIN_SECTION, TITLE_KEY, SPACE);
    inifile.WriteString(INI_MAIN_SECTION, ALBUM_KEY, SPACE);
  end;
  case typeof of
    PSI_NAME: Result := inifile.ReadString(INI_MAIN_SECTION, PSI_NAME_KEY,
                                           PORTABLE_NAME);
    PSI_ARTIST: Result:= inifile.ReadString(INI_MAIN_SECTION, ARTIST_KEY,
                                            SPACE);
    PSI_TITLE: Result:= inifile.ReadString(INI_MAIN_SECTION, TITLE_KEY,
                                           SPACE);
    PSI_ALBUM: Result:= inifile.ReadString(INI_MAIN_SECTION, ALBUM_KEY,
                                           SPACE);
  end;
  inifile.Free;
end;

function GetProcessList(proc_name: String): String; stdcall;
var
  hP: THandle;
  hM: hmodule;
  fprocess: string;
  prcs: array[0..$FFF] of dword;
  cP, cM: cardinal;
  i: integer;
  NameProc: array[0..MAX_PATH] of char;
  Names: array[0..MAX_PATH] of string;
  Pathes: array[0..MAX_PATH] of string;
begin
  if proc_name <> NONE then
  begin
    fprocess := proc_name;
  end
  else
  begin
    fprocess := PORTABLE_NAME;
  end;
  if not EnumProcesses(@prcs, sizeof(prcs), cP) then
  begin
    Result := NONE;
  end;
  for i := 0 to cP div 4 - 1 do
  begin
    hP := OpenProcess(PROCESS_QUERY_INFORMATION or PROCESS_VM_READ,
                      false, prcs[i]);
    if hP <> 0 then
    begin
      EnumProcessModules(hP, @hM, 4, cM);
      GetModuleFileNameEx(hP, hM, NameProc, sizeof(NameProc));
      Names[i] := ExtractFileName(string(NameProc));
      Pathes[i] := string(NameProc);
      CloseHandle(hP);
    end;
  end;
  for i:= 0 to length(Names)-1 do
  begin
    if Names[i] = fprocess then
    begin
      Result:= copy(Pathes[i], 0 , length(Pathes[i]) - length(fprocess));
      Break;
    end
    else
    begin
      Result := NONE;
    end;
  end;
end;

function GetPsiTuneFile(): String; stdcall;
var
  pathList: array [0..4] of String;
  index: integer;
  outPath: String;
begin
  pathList[0] := GetProcessList(GetSettings(PSI_NAME)) + PSIDATA;
  pathList[1] := GetEnvironmentVariable(DATADIR) + SEP + PSIDATA;
  pathList[2] := GetEnvironmentVariable(PROFILE) + XP_SUFFIX + SEP + PSIDATA;
  pathList[3] := GetEnvironmentVariable(LOCALDATA) + SEP + PSIDATA;
  for index := 0 to sizeof(pathList)-1 do
  begin
    outPath := pathList[index];
    if DirectoryExists(outPath) then
    begin
      Break;
    end;
  end;
  if DirectoryExists(outPath) then
  begin
    Result := outPath + TUNEFILE;
  end
  else
  begin
    Result := NONE;
  end;
end;

procedure WriteToFile(FileName: String; title :String; artist :String;
                      album :String; Clear: ShortInt);
var tune_file: TextFile;
begin
  if FileName <> NONE then
  begin
    AssignFile(tune_file, FileName);
    if Clear = WRITE_FILE then
    begin
      Rewrite(tune_file);
      Writeln(tune_file, title);
      Writeln(tune_file, artist);
      Writeln(tune_file, album);
      CloseFile(tune_file);
    end
    else
    begin
      Rewrite(tune_file);
      Writeln(tune_file, NONE);
      CloseFile(tune_file);
    end;
  end;
end;

procedure IsPlay(User, dwCBType: Cardinal); stdcall;
var
  ATune: TAIMP2TuneInfo;
  APls: TAIMP2FileInfo;
  ATitleBuf: array[0..BUF_SIZE] of WideChar;
  AArtistBuf: array[0..BUF_SIZE] of WideChar;
  AAlbumBuf: array[0..BUF_SIZE] of WideChar;
  TuneFilePath: String;
  title, artist, album : WideString;
begin
  try
    ATune:= TAIMP2TuneInfo(User);
    TuneFilePath := GetPsiTuneFile();
    if (ATune <> nil) and (dwCBType = AIMP_PLAY_FILE) then
    begin
      FillChar(APls, SizeOf(APls), 0);
      APls.cbSelfSize := SizeOf(APls);
      APls.sArtist := @AArtistBuf;
      APls.nArtistLen := TAG_SIZE;
      APls.sTitle := @ATitleBuf;
      APls.nTitleLen := TAG_SIZE;
      APls.sAlbum := @AAlbumBuf;
      APls.nAlbumLen := TAG_SIZE;
      if ATune.FAIMPClassic.AIMP_GetCurrentTrack(@APls) then
      begin
        artist := UTF8Encode(AArtistBuf);
        title := UTF8Encode(ATitleBuf);
        album := UTF8Encode(AAlbumBuf);
        if artist = NONE then
        begin
          if GetSettings(PSI_ARTIST) <> NONE then
          begin
            artist := UTF8Encode(GetSettings(PSI_ARTIST));
          end
          else
          begin
            artist := SPACE;
          end;
        end;
        if title = NONE then
        begin
          if GetSettings(PSI_TITLE) <> NONE then
          begin
            title := UTF8Encode(GetSettings(PSI_TITLE));
          end
          else
          begin
            title := SPACE;
          end;
        end;
        if album = NONE then
        begin
          if GetSettings(PSI_ALBUM) <> NONE then
          begin
            album := UTF8Encode(GetSettings(PSI_ALBUM));
          end
          else
          begin
            album := SPACE;
          end;
        end;
        WriteToFile(TuneFilePath, title, artist, album, WRITE_FILE);
      end
      else
        begin
          WriteToFile(TuneFilePath, NONE, NONE, NONE, CLEAR_FILE);
        end;
      end;
  except end;
end;

constructor TAIMP2TuneInfo.Create(AIMP: IAIMP2Controller);
begin
  FAIMPClassic := AIMP;
  if FAIMPClassic <> nil then
  begin
    FAIMPClassic.AIMP_CallBack_Set(AIMP_PLAY_FILE, @IsPlay, Cardinal(Self));
  end;
end;

destructor TAIMP2TuneInfo.Destroy;
begin
  if FAIMPClassic <> nil then
    FAIMPClassic.AIMP_CallBack_Remove(AIMP_PLAY_FILE, @IsPlay);
  FAIMPClassic := nil;
  inherited Destroy;
end;

var ATune: TAIMP2TuneInfo;

procedure TAIMP2TuneInfo.Initialize(AController: IAIMP2Controller);
begin
  ATune := TAIMP2TuneInfo.Create(AController);
end;

procedure TAIMP2TuneInfo.Finalize; stdcall;
var FileName: String;
begin
  FileName:= GetPsiTuneFile();
  if FileExists(FileName) then
  begin
    WriteToFile(FileName, NONE, NONE, NONE, CLEAR_FILE);
    ATune.Free;
  end;
end;

procedure TAIMP2TuneInfo.ShowSettingsDialog(Wind: HWND); stdcall;
var forma: TForm;
begin
  try
    forma:= optDialog.TconfDialog.CreateParented(Wind);
    if forma <> nil then
    begin
      try
        forma.ShowModal;
      finally
        forma.Free;
      end;
    end;
  except end;
end;

function TAIMP2TuneInfo.SetHasSettingsDialog: LongBool; stdcall;
begin
  Result := True;
end;

function TAIMP2TuneInfo.SetPluginAuthor: PWideChar; stdcall;
begin
  Result := AUTHOR;
end;

function TAIMP2TuneInfo.SetPluginName: PWideChar; stdcall;
begin
  Result := PLUGNAME;
end;

function AIMP_QueryAddonEx(out AHeader:IAIMPAddonHeader): LongBool; stdcall;
var AIMP: IAIMP2Controller;
begin
  AHeader := TAIMP2TuneInfo.Create(AIMP);
  Result := True;
end;

exports
  AIMP_QueryAddonEx;

begin

end.
