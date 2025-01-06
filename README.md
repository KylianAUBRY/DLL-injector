# DLL Injector 🛠️💻

A simple DLL injector written in C++ that allows injecting a dynamic link library (.dll) into a specific Windows process.

## ✨ Features
- Finds a target process by its name.
- Allocates memory in the target process to store the DLL path.
- Writes the DLL path into the target process.
- Uses LoadLibraryA to load the DLL into the target process via a remote thread.

## 🚀 Usage

The executable requires two arguments:

1. **Target process name**: Name of the executable file (e.g., `notepad.exe`).
2. **DLL path**: Full or relative path to the DLL file to inject.

### 📝 Example

injector.exe notepad.exe C:\\dlls\\example.dll

## ⚠️ Limitations

- Requires sufficient privileges to access the target process.
- Only compatible with DLLs compiled for the target process's architecture (32-bit or 64-bit).
- **This injector is absolutely not undetectable.**
- Does not handle already injected DLLs (a warning is displayed).

## 📜 Disclaimer

**This project is for educational purposes only.**
The use of techniques such as DLL injection may be considered malicious. Ensure you have proper authorization before using this tool on any third-party system.
