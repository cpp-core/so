// Copyright (C) 2022, 2023 by Mark Melton
//

#include <cassert>
#include <iostream>
#include <string>
#include <fstream>

using std::cout, std::endl;

//This string store the 16 pixels (3 pixels per color, 48 bytes values (code in int main(){}))
std::string rgb_inp;

// Image original size
unsigned short wide_in = 4;
unsigned short height_in = 4;

// Output image size
unsigned short wide_end = 1920;
unsigned short height_end = 1080;

// This string stores all the image bytes during the interpolation (1920*1080*3 bytes)
std::string rgb_out((wide_end * 3)* height_end, '\0');

// Header of the BMP file (necessary to make the BMP file to be readable)
unsigned char enc_bmp[54] = {0x42, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x80, 0x07, 0x00, 0x00, 0x38, 0x04, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Define limits for RGB values (0 to 255) and the for the 16 neighboor pixels in the 4x4 original image grid (coordinates)
float def_lims(float val_var, float val_max)
{
    if (val_var < 0)
    {
        val_var = 0;
    }
    else if (val_var > val_max)
    {
        val_var = val_max;
    }

    return val_var;
};

// Interpolation formulas
float bicub_hermite(float f_p0, float f_p1, float f_p2, float f_p3, float xy_frac)
{
    float coef_a = -(0.5f * f_p0) + (1.5f * f_p1) - (1.5f * f_p2) + (0.5f * f_p3);
    float coef_b = (f_p0)-(2.5f * f_p1) + (2.0f * f_p2) - (0.5f * f_p3);
    float coef_c = -(0.5f * f_p0) + (0.5f * f_p2);
    float coef_d = f_p1;

    return (coef_a * (xy_frac * xy_frac * xy_frac)) + (coef_b * (xy_frac * xy_frac)) + (coef_c * (xy_frac)) + (coef_d);
};

// Get the memory direction for the pixel in the original image with the normalized coordinates
unsigned char* pixel_lims(short x_ent2, short y_ent2)
{
    // Define limit values for the 16 neighboor pixels in the 4x4 original image grid
    x_ent2 = def_lims(x_ent2, wide_in);
    y_ent2 = def_lims(y_ent2, height_in);

    // Return memory direction (based on the integer value of the normalized coordinate) in it's first RGB value in the input image
    return reinterpret_cast<unsigned char *>(&rgb_inp[(y_ent2 * (wide_in * 3)) + (x_ent2 * 3)]);
};

// Normalize coordinates (proportionally) and store the 3 RGB values in a string
std::string pixel_interp(float& cord_u2, float& cord_v2)
{
    // Normalize coordinates and make x_frac and y_frac the fractional part of the normalized coordinate (xy_frac in bicub_hermite)
    float cord_x = (cord_u2 * wide_in) - 0.5;
    short x_ent = int(cord_x);
    // Fractional part of the coordinate
    float x_frac = cord_x - x_ent;

    float cord_y = (cord_v2 * height_in) - 0.5;
    short y_ent = int(cord_y);
    float y_frac = cord_y - y_ent;

    // 16 neighboor pixels (4 rows in total of 4 pixels every row), first row
    unsigned char* p00 = pixel_lims(x_ent - 1, y_ent - 1);
    unsigned char* p10 = pixel_lims(x_ent + 0, y_ent - 1);
    unsigned char* p20 = pixel_lims(x_ent + 1, y_ent - 1);
    unsigned char* p30 = pixel_lims(x_ent + 2, y_ent - 1);

    // Second row
    unsigned char* p01 = pixel_lims(x_ent - 1, y_ent + 0);
    unsigned char* p11 = pixel_lims(x_ent + 0, y_ent + 0); // Origin coordinate
    unsigned char* p21 = pixel_lims(x_ent + 1, y_ent + 0);
    unsigned char* p31 = pixel_lims(x_ent + 2, y_ent + 0);

    unsigned char* p02 = pixel_lims(x_ent - 1, y_ent + 1);
    unsigned char* p12 = pixel_lims(x_ent + 0, y_ent + 1);
    unsigned char* p22 = pixel_lims(x_ent + 1, y_ent + 1);
    unsigned char* p32 = pixel_lims(x_ent + 2, y_ent + 1);

    unsigned char* p03 = pixel_lims(x_ent - 1, y_ent + 2);
    unsigned char* p13 = pixel_lims(x_ent + 0, y_ent + 2);
    unsigned char* p23 = pixel_lims(x_ent + 1, y_ent + 2);
    unsigned char* p33 = pixel_lims(x_ent + 2, y_ent + 2);

    if (cord_u2 == float(2.0)/float(wide_end-1) and cord_v2 == float(2.0)/float(height_end-1)) {
	cout << x_frac << " " << y_frac << endl;
	cout << int(p00[0]) << " ";
	cout << int(p10[0]) << " ";
	cout << int(p20[0]) << " ";
	cout << int(p30[0]) << endl;
	
	cout << int(p01[0]) << " ";
	cout << int(p11[0]) << " ";
	cout << int(p21[0]) << " ";
	cout << int(p31[0]) << endl;
	
	cout << int(p02[0]) << " ";
	cout << int(p12[0]) << " ";
	cout << int(p22[0]) << " ";
	cout << int(p32[0]) << endl;
	
	cout << int(p03[0]) << " ";
	cout << int(p13[0]) << " ";
	cout << int(p23[0]) << " ";
	cout << int(p33[0]) << endl;
    }

    // Store the interpolated pixel (3 interpolation process, one per RGB layer)
    std::string interp_2d_pixel(3, '\0');
    for (unsigned int i = 0; i < 3; ++i)
    {
        // Interpolate vertically
        float fila_0 = bicub_hermite(p00[i], p10[i], p20[i], p30[i], x_frac);
        float fila_1 = bicub_hermite(p01[i], p11[i], p21[i], p31[i], x_frac);
        float fila_2 = bicub_hermite(p02[i], p12[i], p22[i], p32[i], x_frac);
        float fila_3 = bicub_hermite(p03[i], p13[i], p23[i], p33[i], x_frac);

        // Interpolate horizontally
        float pixel_val = bicub_hermite(fila_0, fila_1, fila_2, fila_3, y_frac);

        // Define maximum value for in the RGB scale
        pixel_val = def_lims(pixel_val, 255.0f);

        // Store the value in the 3 layer RGB pixel
        interp_2d_pixel[i] = static_cast<unsigned char>(pixel_val);
    }

    // Return the string of 3 RGB values
    return interp_2d_pixel;
};

// Iterate and store values for the output rgb string (output image of 1920 rows x 1080 columns)
void redim_img()
{
    // Iterator for the Y axis
    char* it_rgb_out_y = &rgb_out[0];

    for (unsigned short filas_y = 0; filas_y < height_end; ++filas_y)
    {
        // Iterator for the X axis
        char* it_rgb_out_x = it_rgb_out_y;

        // cord_v is the regular fraction value in the Y axis (1/1920 * iteration number)
        float cord_v = float(filas_y) / float(height_end - 1);

        for (unsigned short columnas_x = 0; columnas_x < wide_end; ++columnas_x)
        {
            // cord_u is the regular fraction value in the X axis (1/1080 * iteration number)
            float cord_u = float(columnas_x) / float(wide_end - 1);
            std::string pixel_rgb_out(3, '\0');

            // Call to function pixel_interp
            pixel_rgb_out = pixel_interp(cord_u, cord_v);

            // Store the 3 RGB values
            it_rgb_out_x[0] = pixel_rgb_out[0];
            it_rgb_out_x[1] = pixel_rgb_out[1];
            it_rgb_out_x[2] = pixel_rgb_out[2];

            // Move the iterator 3 positions (1 pixel)
            it_rgb_out_x += 3;
        }

        // Move the iterator 1920*3 positions (a row)
        it_rgb_out_y += wide_end * 3;
    }
};

int main()
{
    for (auto i = 0; i < 4; ++i) {
	for (auto j = 0; j < 4; ++j) {
	    auto value = (i == 0 and j == 0) ? 0x00 : 0xFF;
	    rgb_inp.push_back(value);
	    rgb_inp.push_back(value);
	    rgb_inp.push_back(value);
	}
    }

    // Insert the 16 pixels
    // rgb_inp.push_back(0x00);
    // rgb_inp.push_back(0x00);
    // rgb_inp.push_back(0x00);
    
    // rgb_inp.push_back(0xA0);
    // rgb_inp.push_back(0xA0);
    // rgb_inp.push_back(0xA0);

    // rgb_inp.push_back(0x50);
    // rgb_inp.push_back(0x50);
    // rgb_inp.push_back(0x50);
    
    // rgb_inp.push_back(0xF0);
    // rgb_inp.push_back(0xF0);
    // rgb_inp.push_back(0xF0);
    
    // rgb_inp.push_back(0x00);
    // rgb_inp.push_back(0x00);
    // rgb_inp.push_back(0x00);

    // rgb_inp.push_back(0x50);
    // rgb_inp.push_back(0x50);
    // rgb_inp.push_back(0x50);

    // rgb_inp.push_back(0xF0);
    // rgb_inp.push_back(0xF0);
    // rgb_inp.push_back(0xF0);
    
    // rgb_inp.push_back(0x00);
    // rgb_inp.push_back(0x00);
    // rgb_inp.push_back(0x00);

    // rgb_inp.push_back(0xA0);
    // rgb_inp.push_back(0xA0);
    // rgb_inp.push_back(0xA0);

    // rgb_inp.push_back(0xA0);
    // rgb_inp.push_back(0xA0);
    // rgb_inp.push_back(0xA0);
    
    // rgb_inp.push_back(0xF0);
    // rgb_inp.push_back(0xF0);
    // rgb_inp.push_back(0xF0);

    // rgb_inp.push_back(0x50);
    // rgb_inp.push_back(0x50);
    // rgb_inp.push_back(0x50);

    // rgb_inp.push_back(0x00);
    // rgb_inp.push_back(0x00);
    // rgb_inp.push_back(0x00);
    
    // rgb_inp.push_back(0xA0);
    // rgb_inp.push_back(0xA0);
    // rgb_inp.push_back(0xA0);
    
    // rgb_inp.push_back(0x50);
    // rgb_inp.push_back(0x50);
    // rgb_inp.push_back(0x50);
    
    // rgb_inp.push_back(0xF0);
    // rgb_inp.push_back(0xF0);
    // rgb_inp.push_back(0xF0);

    // Store the BMP header to the output image
    std::string bmp_out;

    for (unsigned char ch_ch : enc_bmp)
    {
        bmp_out.push_back(ch_ch);
    }

    // Start of the algorithm
    redim_img();

    // Store all the RGB values (pixels) from rgb_out string in the output image
    for (unsigned char ch_ch : rgb_out)
    {
        bmp_out.push_back(ch_ch);
    }

    // Save the image
    std::ofstream bmp_arch("/tmp/x_0.bmp", std::ios::binary);
    bmp_arch.write(bmp_out.data(), bmp_out.size());
    bmp_arch.close();
    std::cout << "Finished";

    return 0;
}
