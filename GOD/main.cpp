#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct tga_header {
    uint8_t ID_field_length, Colormap_type, Image_type;
    uint16_t Colormap_offset, Colormap_length;
    uint8_t Colormap_entry_size;
    uint16_t X_origin, Y_origin, Image_width, Image_height;
    uint8_t Pixel_depth_bpp, Image_descriptor;

    tga_header() {
        ID_field_length = 0; // Initialize to 0, can be changed
        Colormap_type = 0; // File contains no color map
        Image_type = 2; // uncompressed true-color image
        Colormap_offset = Colormap_length = 0;
        Colormap_entry_size = 0; // Unset, as there's no colormap. 0 is ok ??
        X_origin = Y_origin = 0; // I wonder what these do
        Pixel_depth_bpp = 24; // Default to 24 bpp
        Image_descriptor = 0;
    }
};

struct tga_footer {
    uint32_t Extension_offset, Devarea_offset;

    tga_footer() { Extension_offset = Devarea_offset = 0; }
};

std::ostream &operator<<(std::ostream &os, tga_header &hdr) {
    os << hdr.ID_field_length<< hdr.Colormap_type << hdr.Image_type;
    os.write((const char *)&hdr.Colormap_offset, 2);
    os.write((const char *)&hdr.Colormap_length, 2);
    os << hdr.Colormap_entry_size;
    os.write((const char *)&hdr.X_origin, 2);
    os.write((const char *)&hdr.Y_origin, 2);
    os.write((const char *)&hdr.Image_width, 2);
    os.write((const char *)&hdr.Image_height, 2);
    os << hdr.Pixel_depth_bpp << hdr.Image_descriptor;
    return os;
}

std::ostream &operator<<(std::ostream &os, tga_footer &ftr) {
    os.write((const char *)&ftr.Extension_offset, 4);
    os.write((const char *)&ftr.Devarea_offset, 4);
    os << "TRUEVISION-XFILE.";
    os.write("\0", 1);
    return os;
}

int save_tga(std::string filename, uint8_t *data, int w, int h) {
    std::string ID_field(""); //"Created by GOD");
    tga_header hdr;
    tga_footer ftr;

    hdr.ID_field_length = ID_field.length() + 1;
    hdr.Image_width = w;
    hdr.Image_height = h;

    std::ofstream savestream(filename.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
    savestream << hdr << ID_field;
    savestream.write("\0", 1);
    savestream.write((const char *)data, w*h*3);
    savestream << ftr;
    savestream.close();
    return 0;
}


const int w = 800, h = 800;
uint8_t mapImage[w*h*3];
std::string mapName("TestMap1");

inline void putpixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    mapImage[(x+y*w)*3 + 0] = b;
    mapImage[(x+y*w)*3 + 1] = g;
    mapImage[(x+y*w)*3 + 2] = r;
}


int main()
{
    int i,j;
    std::string tmpstr;

    for (j=0; j<h; j++) for (i=0; i<w; i++) putpixel(i, j, 64, 64 + i & 127, 64 + j & 127);
    for (j=0; j<4; j++) for (i=0; i<4; i++) putpixel(20+200*i, 40+160*j, 255, 255, 255);
    for (j=0; j<3; j++) for (i=0; i<w; i++) putpixel(i, 120+160*j, 255, 0, 0);
    for (j=0; j<h; j++) for (i=0; i<3; i++) putpixel(120+200*i, j, 255, 0, 0);

    save_tga(mapName + ".tga", mapImage, w, h);

    tmpstr = mapName + ".map";
    std::ofstream mapFile(tmpstr.c_str());
    mapFile << "#dom2title " << mapName << std::endl;
    mapFile << "#imagefile " << mapName << ".tga" << std::endl;
    mapFile << "#domversion 175" << std::endl;
    mapFile << "#description \"" << "Test map created by GOD!" << "\"" << std::endl;
    for (i=1; i<17; i++) {
        mapFile << "#landname "<<i<<" name"<<i << std::endl;
        mapFile << "#terrain "<<i<<" 0" << std::endl;
    }
    for (i=1; i<16; i++) mapFile << "#neighbour "<<i<<" "<<i+1<<std::endl;
    mapFile << "#start 1"<<std::endl;
    mapFile << "#start 14"<<std::endl;
    mapFile.close();

    cout << "Hello world!" << endl;
    return 0;
}
