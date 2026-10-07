# Omnis — Visual Asset Boundary

This folder is the source boundary for authored visual assets.

The first vertical slice deliberately uses Unreal Engine 5.6 engine-native geometry at runtime so the project remains buildable without a third-party asset pack.

Persistent project-specific visual assets should be imported into Unreal and saved as .uasset packages under Content/Omnis. Levels are saved as .umap packages under Content/Omnis/Maps.

Use Tools/Editor/GenerateOmnisVerticalSlice.py to author the first real map package.
