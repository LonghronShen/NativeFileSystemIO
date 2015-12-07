VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Begin VB.Form FormMain 
   Caption         =   "Native File Copy Demo for VB6"
   ClientHeight    =   2370
   ClientLeft      =   120
   ClientTop       =   465
   ClientWidth     =   6645
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   ScaleHeight     =   2370
   ScaleWidth      =   6645
   StartUpPosition =   2  'CenterScreen
   Begin MSComDlg.CommonDialog CD 
      Left            =   5880
      Top             =   1800
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.CommandButton BtnExit 
      Caption         =   "Exit"
      Height          =   615
      Left            =   3720
      TabIndex        =   7
      Top             =   1440
      Width           =   1335
   End
   Begin VB.CommandButton BtnCopy 
      Caption         =   "Copy"
      Height          =   615
      Left            =   1200
      TabIndex        =   6
      Top             =   1440
      Width           =   1335
   End
   Begin VB.CommandButton BtnBrowseTarget 
      Caption         =   "Browse"
      Height          =   255
      Left            =   5400
      TabIndex        =   5
      Top             =   840
      Width           =   975
   End
   Begin VB.CommandButton BtnBrowseSource 
      Caption         =   "Browse"
      Height          =   255
      Left            =   5400
      TabIndex        =   4
      Top             =   240
      Width           =   975
   End
   Begin VB.TextBox TB_Target 
      Height          =   285
      Left            =   1320
      TabIndex        =   3
      Top             =   840
      Width           =   3735
   End
   Begin VB.TextBox TB_Source 
      Height          =   285
      Left            =   1320
      TabIndex        =   1
      Top             =   240
      Width           =   3735
   End
   Begin VB.Label Label2 
      Caption         =   "Target file"
      Height          =   255
      Left            =   240
      TabIndex        =   2
      Top             =   840
      Width           =   975
   End
   Begin VB.Label Label1 
      Caption         =   "Source file"
      Height          =   255
      Left            =   240
      TabIndex        =   0
      Top             =   240
      Width           =   975
   End
End
Attribute VB_Name = "FormMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Declare Function FileCopy Lib "NativeFileSystemIO.dll" (ByVal lpSrcName As String, ByVal lpDstName As String) As Long
Private Declare Function GetProgramBits Lib "NativeFileSystemIO.dll" () As Long
Private Declare Function GetSystemBits Lib "NativeFileSystemIO.dll" () As Long

Private Declare Function Wow64DisableWow64FsRedirection Lib "Kernel32" (ByRef ptr As Long) As Boolean
Private Declare Function Wow64RevertWow64FsRedirection Lib "Kernel32" (ByVal ptr As Long) As Boolean

Private Sub BtnBrowseSource_Click()
    With CD
        .DialogTitle = "Open a source file"
        Call .ShowOpen
        Me.TB_Source.Text = .FileName
    End With
End Sub

Private Sub BtnBrowseTarget_Click()
    With CD
        .DialogTitle = "Save as"
        Call .ShowSave
        Me.TB_Target.Text = .FileName
    End With
End Sub

Private Sub BtnCopy_Click()
    On Error GoTo Err
    If GetSystemBits() = 64 Then
        Dim oldValue As Long
        If Wow64DisableWow64FsRedirection(oldValue) = False Then
            MsgBox ("Failed to shutdown the Wow64FsRedirection function.")
            Exit Sub
        End If
    End If
    If FileCopy(Me.TB_Source.Text, Me.TB_Target.Text) = 0 Then
        MsgBox ("Failed to copy the file!")
    Else
        MsgBox ("Successfully copied the file!")
    End If
    If GetSystemBits() = 64 Then
        If Wow64RevertWow64FsRedirection(oldValue) = False Then
            MsgBox ("Failed to revert the Wow64FsRedirection function.")
            Exit Sub
        End If
    End If
    Exit Sub
Err:
    MsgBox (Err.Description)
End Sub

Private Sub BtnExit_Click()
    End
End Sub

Private Sub Form_Load()
    With CD
        .Filter = "All files (*.*)|*.*"
    End With
    Me.TB_Source.Text = "C:\Windows\System32\notepad.exe"
    Me.TB_Target.Text = "D:\temp\system"
End Sub
