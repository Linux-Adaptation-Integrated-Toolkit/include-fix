# include-fix

## English

this repo is part of Integrated Toolkit, Its function is to search the appropriate solution through the package management system after detecting the missing header file of the C / CPP source file.

Here are three solutions:

* If these dependent libraries (including header files and dynamic / static link libraries) have been installed in the operating system through the package management system, they will be retrieved (the implementation code refers to dpkg - s).

* If these dependent libraries are not found through the above detection, search in the remote warehouse through the package management system (refer to the implementation principle of apt file search)

* If the required dependent libraries cannot be found in the above two methods, it can almost be considered that the libraries do not exist in., / usr / include and / usr / local / include. At this time, there is only one possibility: the required dependent libraries are either installed, but installed in some strange paths, or not installed locally, and there is no remote warehouse, which can only be solved by the developers themselves.

### compile

```
mkdir build && cd build && cmake ..
make -j()
```

## Chinese

它的作用是在检测到c/cpp源码文件缺失头文件后，通过包管理系统去搜索恰当的解决方案。

* 这里有三个解决方案：

* 如果这些依赖库（包含头文件和动态/静态链接库）已通过包管理系统安装在操作系统中，则进行检索（实现代码大量参考了dpkg -S）。

* 如果这些依赖库通过以上检测未找到，则通过包管理系统在远程仓库中搜索（需要参考apt-file search实现原理）

如果以上两个办法都找不到需要的依赖库，那么几乎可以认为这个库在. 、/usr/include和/usr/local/include是不存在的，这时只有一种可能：需要的依赖库要么已安装，但安装在了某些奇怪的路径，或本地未安装，同时远程仓库也没有，这样只能依靠开发者自己去解决。
