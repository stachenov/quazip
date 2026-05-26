# Release process

# Compatibility policy
- **Both API and ABI** are stable within a minor version (any breaking change requires at least a minor bump)

# Release process

1. In `CMakeLists.txt` change `VERSION` (SOVERSION is derived automatically as MAJOR.MINOR)
2. Change version in `vcpkg.json`
3. Create a new tag
4. Create a GH release based on the tag