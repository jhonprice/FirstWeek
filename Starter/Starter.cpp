// Starter.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "vec3.h" //2
#include "color.h" //2

//<<输出一张PPM格式的图片>>
void drawOnePPM() {
//<<设置Image的长度与宽度，以像素为单位>>
#pragma region ImageSize 
	const int image_width = 256;
	const int image_height = 256;
#pragma endregion

//<<按照PPM的格式打印字符串>>
/*
*	期望输出: 红色从左到右从完全关闭（黑色）变为完全打开（亮红色），
			绿色变为 从底部的黑色到顶部的完全打开。 红色和绿色一起变成黄色，
			所以我们 应该期望右上角是黄色的。 
*/
#pragma region RenderPart
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
	//<<添加进度指示的渲染循环>> - 1
	//<<利用头文件vec3 color工具函数简化过程>> - 2
	for (int j = image_height - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush; //1
		for (int i = 0; i < image_width; ++i) {
			auto r = double(i) / (image_width - 1);
			auto g = double(j) / (image_height - 1);
			auto b = 0.25;

			//2 int ir = static_cast<int>(255.999 * r);
			//2 int ig = static_cast<int>(255.999 * g);
			//2 int ib = static_cast<int>(255.999 * b);

			//2 std::cout << ir << ' ' << ig << ' ' << ib << '\n';

			color pixel_color(r,g,b); //2
			write_color(std::cout, pixel_color); //2
			
		}
		std::cerr << "\nDone.\n";  //1
	}
#pragma endregion

}

int main()
{
	drawOnePPM();
}