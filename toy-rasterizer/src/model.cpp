#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char* filename) : verts_(), faces_(), norms_(), uv_()
{
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (in.fail()) return;
	std::string line;
	while (!in.eof())
	{
		std::getline(in, line);
		std::istringstream iss(line.c_str());
		char trash;
		
		// Store vertices
		if (!line.compare(0, 2, "v ")) // compare() return integer! :: 0 means equal
		{
			iss >> trash;
			Vec3f v;
			for (int i = 0; i < 3; i++) iss >> v[i];
			verts_.push_back(v);
		}
		// Store normal
		else if (!line.compare(0, 3, "vn "))
		{
			iss >> trash >> trash; // delete char v, char n
			Vec3f n;
			for (int i = 0; i < 3; i++) iss >> n[i];
			norms_.push_back(n);
		}
		// Store texture vertices ( u, v with [0,1] )
		else if (!line.compare(0, 3, "vt "))
		{
			iss >> trash >> trash;
			Vec2f t;
			for (int i = 0; i < 2; i++) iss >> t[i];
			uv_.push_back(t);
		}
		// Format : f v/vt/vn/v/vt/vn/v/vt/vn by index of v, vt, vn
		else if (!line.compare(0, 2, "f "))
		{
			std::vector<Vec3i> f;
			Vec3i temp;
			iss >> trash;
			while (iss >> temp[0] >> trash >> temp[1] >> trash >> temp[2])
			{
				for (int i = 0; i < 3; i++) temp[i]--; // in wavefront obj all indices start at 1, not 0
				f.push_back(temp);
			}
			faces_.push_back(f);
		}
	}
	std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << std::endl;
	load_texture(filename, "_diffuse.tga", diffusemap_);
}

Model::~Model() {}

int Model::nverts() { return (int)verts_.size(); }

int Model::nfaces() { return (int)faces_.size(); }

std::vector<int> Model::face(int idx) 
{	// Format : f v/vt/vn/v/vt/vn/v/vt/vn --> abstract only v
	// present status of face[i] :: [0] : v,vt,vn , [1] : v,vt,vn, [2] : v,vt,vn

	std::vector<int> face;
	for (int i = 0; i < (int)faces_[idx].size(); i++)
	{
		face.push_back(faces_[idx][i][0]);
	}
	return face; 
}

Vec3f Model::vert(int i) { return verts_[i]; }

void Model::load_texture(std::string filename, const char* suffix, TGAImage& img)
{
	std::string textfile(filename);
	size_t dot = textfile.find_last_of(".");
	if (dot != std::string::npos)
	{
		textfile = textfile.substr(0, dot) + std::string(suffix);
		std::cerr << "texture file " << textfile << " loading " << (img.read_tga_file(textfile.c_str()) ? "ok" : "failed") << std::endl;
		img.flip_vertically();
	}
}

TGAColor Model::diffuse(Vec2i uv)
{
	return diffusemap_.get(uv.x, uv.y); 
}

Vec2i Model::uv(int iface, int nvert) // find texture's coords in u,v
{
	int idx = faces_[iface][nvert][1];
	return Vec2i(uv_[idx].x * diffusemap_.get_width(), uv_[idx].y * diffusemap_.get_height()); //implicit casting float to int
}

Vec3f Model::norm(int iface, int nvert)
{
	int idx = faces_[iface][nvert][2];
	return norms_[idx].normalize();
}



