# UE4TileBasedMovement

This project uses a customizable Instanced Static Mesh grid, as well as multiple components that
build up functionality. 

Instanced Tile Grid
- BP that inherits from the C++ implementation.
- can be modified dimension wise. R x C. 
- Tile offset can also be changed.

Tile Location Component
- Stores location and ObjectIdentity information
- all actors that want to be snapped to a tile use this.

Tile Movement Component
- Stores movement info, and provides TArrays of valid tiles.

Tile Attack Component
- Stores attack infomation in FMoveInformation and FRangeInfo structs.
- provides TArrays of valid attack tiles.
