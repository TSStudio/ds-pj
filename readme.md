# TSMap

Project for the course "COMP130004h - Data Structure" at Fudan University, 2024 Fall.

Sub repos for the project:

-   **Backend #1**, main part of the project, written in C++, implementing the data structure and the algorithm
-   Backend #2, written in PHP, providing supporting services for the frontend (e.g. Node saving)
-   Frontend, written in Vue.js, providing the user interface for the project
-   Renderer, written in PHP, providing SVG rendering for each tile in the map

# Backend #1

## License

This project is licensed under the GNU General Public License v3.0.

**You are free to use, modify, and distribute this project, as long as you follow the license.**

If you are a student in Fudan University: **As the rules and regulations of Fudan University, you are not allowed to copy anything from this repo in your project in the same course.**

## To develop

Environment requirements: Linux x86_64, CMake, GNU GCC (14 or above)

Install Crow, following this instruction:

https://crowcpp.org/master/getting_started/setup/linux/

Install unordered_dense, following this instruction:

(https://github.com/martinus/unordered_dense?tab=readme-ov-file#2-installation)

Install boost, following this instruction:

https://www.boost.org/doc/libs/1_87_0/more/getting_started/unix-variants.html

Also, you need to put `nlohmann_json` and `pugixml` in the project directory.

Run `cmake .` to generate Makefile.
Run `make` to compile. If you want to compile with multiple threads, you can use `make -j`.


# TSMap

复旦大学 2024-2025 秋季学期《数据结构》课程项目。

项目的子仓库：

-   **后端 #1**，项目的主要部分，使用 C++ 编写，实现数据结构和算法
-   后端 #2，使用 PHP 编写，为前端提供支持服务（例如节点保存）
-   前端，使用 Vue.js 编写，为项目提供用户界面
-   渲染器，使用 PHP 编写，为地图中的每个瓦片提供 SVG 渲染

# 后端 #1

## 许可证

本项目使用 GNU 通用公共许可证 v3.0 进行许可。

**您可以自由使用、修改和分发本项目，只要您遵守许可证即可。**

如果您是复旦大学的学生：**根据复旦大学的规定，您不得在同一课程的项目中从此存储库中复制任何内容。**

## 开发

环境要求：Linux x86_64，CMake，GNU GCC（14 或以上）

安装 Crow，依据此说明：

https://crowcpp.org/master/getting_started/setup/linux/

安装 unordered_dense，依据此说明：

https://github.com/martinus/unordered_dense?tab=readme-ov-file#2-installation

安装 boost，依据此说明：

https://www.boost.org/doc/libs/1_87_0/more/getting_started/unix-variants.html

另外，需要将 `nlohmann_json` 和 `pugixml` 放在项目目录下。

运行 `cmake .` 生成 Makefile。
运行 `make` 编译。若使用多线程编译，可以使用 `make -j`。