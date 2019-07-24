

### NOTE
// must give a define , before use stb_image.h
```c
#define STB_IMAGE_IMPLEMENTATION
#include "../../third_part_src/stb_image.h"
```

// must add -std=c++11, before compile assimp lib and apps
```c
g++ -o $1.out $1.cpp ../../third_part_src/glad.c -lglfw3 -lGL -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -ldl -lXinerama -lXcursor -lassimp -std=c++11
```

