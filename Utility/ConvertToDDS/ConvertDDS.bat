for %%f in (*.png) do (
  .\texconv.exe -f R8G8B8A8_UNORM -srgbi -ft dds "%%~f"
)
