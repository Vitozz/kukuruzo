unit optDialog;

interface

uses
  Windows,
  Controls,
  Forms,
  StdCtrls,
  IniFiles,
  Classes;

type
    TconfDialog = class(TForm)
        processname1: TEdit;
        Label1: TLabel;
        Label2: TLabel;
        artistTag: TEdit;
        Label3: TLabel;
        titleTag: TEdit;
        Label4: TLabel;
        albumTag: TEdit;
        saveButton: TButton;
        cancelButton: TButton;
        procedure saveButtonClick(Sender: TObject);
        procedure cancelButtonClick(Sender: TObject);
        procedure FormActivate(Sender: TObject);
    private
        filePath: String;
        const INI_FILE_NAME = '\aimp_psi_tune.ini';
        const INI_MAIN_SECTION = 'Tune';
        const DIALOG_CAPTION = 'TuneToPsi Options';
        const PSI_NAME_KEY = 'psi_name';
        const PSI_DEFAULT_NAME = 'psi-plus-portable.exe';
        const ARTIST_KEY = 'arttag';
        const TITLE_KEY = 'titltag';
        const ALBUM_KEY = 'albtag';
        { Private declarations }
    public
        { Public declarations }
    end;

var
  confDialog: TconfDialog;

implementation
{$R *.dfm}
uses SysUtils;
procedure TconfDialog.saveButtonClick(Sender: TObject);
var inifile: TIniFile;
begin
    if filePath <> '' then
    begin
      inifile:= TIniFile.Create(filePath + INI_FILE_NAME);
      inifile.WriteString(INI_MAIN_SECTION, PSI_NAME_KEY, processname1.Text);
      inifile.WriteString(INI_MAIN_SECTION, ARTIST_KEY, artistTag.Text);
      inifile.WriteString(INI_MAIN_SECTION, TITLE_KEY, titleTag.Text);
      inifile.WriteString(INI_MAIN_SECTION, ALBUM_KEY, albumTag.Text);
      inifile.Free;
      Close;
    end;
end;

procedure TconfDialog.cancelButtonClick(Sender: TObject);
begin
    Close;
end;

procedure TconfDialog.FormActivate(Sender: TObject);
var inifile: TIniFile;
begin
    filePath := GetCurrentDir();
    inifile:= TIniFile.Create(filePath + INI_FILE_NAME);
    self.Caption:= DIALOG_CAPTION;
    processname1.Text:= inifile.ReadString(INI_MAIN_SECTION, PSI_NAME_KEY, PSI_DEFAULT_NAME);
    artistTag.Text:= inifile.ReadString(INI_MAIN_SECTION, ARTIST_KEY, ' ');
    titleTag.Text:= inifile.ReadString(INI_MAIN_SECTION, TITLE_KEY, ' ');
    albumTag.Text:= inifile.ReadString(INI_MAIN_SECTION, ALBUM_KEY, ' ');
    inifile.Free;
end;

end.
