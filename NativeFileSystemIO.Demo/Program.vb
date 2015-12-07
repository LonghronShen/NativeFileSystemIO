Module Program

    Public Declare Function FileCopy Lib "NativeFileSystemIO.dll" (ByVal lpSrcName As String, ByVal lpDstName As String) As Integer

    Public Declare Function GetProgramBits Lib "NativeFileSystemIO.dll" () As Integer

    Public Declare Function GetSystemBits Lib "NativeFileSystemIO.dll" () As Integer

    Public Declare Function Wow64DisableWow64FsRedirection Lib "Kernel32" (ByRef ptr As IntPtr) As Boolean

    Public Declare Function Wow64RevertWow64FsRedirection Lib "Kernel32" (ByVal ptr As IntPtr) As Boolean

    Public Function ExecutingFsOperationWithoutWow64FsRedirection(Of T)(ByVal operation As Func(Of T)) As T
        If Environment.Is64BitOperatingSystem And Environment.Is64BitProcess = False Then
            Console.WriteLine("Detected that you are running x86 version on a x64 operating system." + vbCrLf + "Trying to shutdown the Wow64FsRedirection function...")
            Dim oldValue = IntPtr.Zero
            If Wow64DisableWow64FsRedirection(oldValue) = True Then
                Console.WriteLine("Successfully shutdown the Wow64FsRedirection function. Old value is " + oldValue.ToString())
                Dim result = operation()
                If Wow64RevertWow64FsRedirection(oldValue) = False Then
                    Console.WriteLine("Failed to revert Wow64FsRedirection state.")
                Else
                    Return result
                End If
            Else
                Console.WriteLine("Failed to shutdown the Wow64FsRedirection function. Old value is " + oldValue.ToString())
            End If
        Else
            Return operation()
        End If
    End Function

    Sub Main()
        Console.WriteLine("Please input the source file path:" + vbCrLf + "[C:\Windows\System32\config\SYSTEM]")
        Dim source = Console.ReadLine()
        If String.IsNullOrEmpty(source) Then source = "C:\Windows\System32\notepad.exe" ' "C:\Windows\System32\config\SYSTEM"
        Console.WriteLine("Please input the target file path:" + vbCrLf + "[D:\temp\system]")
        Dim target = Console.ReadLine()
        If String.IsNullOrEmpty(target) Then target = "D:\temp\system"
        Console.WriteLine("Trying to copy {0} to {1}...", source, target)
        If ExecutingFsOperationWithoutWow64FsRedirection(Function() FileCopy(source, target)) = 0 Then
            Console.WriteLine("Failed to copy the source file {0} to target {1}.", source, target)
        Else
            Console.WriteLine("Successfully copied the source file {0} to target {1}.", source, target)
        End If
Err:
        Console.WriteLine("Press any key to continue.")
        Console.ReadKey(False)
    End Sub

End Module