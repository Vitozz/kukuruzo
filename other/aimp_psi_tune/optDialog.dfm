object confDialog: TconfDialog
  Left = 820
  Top = 690
  BorderStyle = bsDialog
  Caption = 'Options'
  ClientHeight = 207
  ClientWidth = 247
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poScreenCenter
  OnActivate = FormActivate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 127
    Height = 13
    Caption = 'Psi-portable process name:'
  end
  object Label2: TLabel
    Left = 8
    Top = 48
    Width = 83
    Height = 13
    Caption = 'Artist tag if empty:'
  end
  object Label3: TLabel
    Left = 8
    Top = 88
    Width = 80
    Height = 13
    Caption = 'Title tag if empty:'
  end
  object Label4: TLabel
    Left = 8
    Top = 128
    Width = 86
    Height = 13
    Caption = 'Album tag if empty'
  end
  object processname1: TEdit
    Left = 8
    Top = 24
    Width = 233
    Height = 21
    TabOrder = 0
    Text = 'psi-portable.exe'
  end
  object artistTag: TEdit
    Left = 8
    Top = 64
    Width = 233
    Height = 21
    TabOrder = 1
    Text = 'Unknown(Radio)'
  end
  object titleTag: TEdit
    Left = 8
    Top = 104
    Width = 233
    Height = 21
    TabOrder = 2
    Text = 'Unknown'
  end
  object albumTag: TEdit
    Left = 8
    Top = 144
    Width = 233
    Height = 21
    TabOrder = 3
    Text = 'Unknown'
  end
  object saveButton: TButton
    Left = 24
    Top = 176
    Width = 75
    Height = 25
    Caption = 'Save'
    TabOrder = 4
    OnClick = saveButtonClick
  end
  object cancelButton: TButton
    Left = 144
    Top = 176
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 5
    OnClick = cancelButtonClick
  end
end
