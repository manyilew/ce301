#ifndef TEXTURE_H
#define TEXTURE_H

#ifdef WIN32
#include <glut.h>
#else
#include <GL/glut.h> //Should be Mac OS X standard in Xcode projects.
#endif

#include <stdlib.h>
#include <stdio.h>

#define GL_CLAMP_TO_EDGE 0x812F

//Window Texture 
#define WINDOW_HEIGHT 10
#define WINDOW_WIDTH 8
#define WINDOW_BEZEL 2

//Random Color Variance Range
#define COLOR_RANGE 30

//Lights
#define UNLIT 10
#define DIMLIT 80
#define WELL_LIT 160
#define BRIGHT 245

//Features
#define MIPMAP 1
#define ANISOTROPIC 1

enum TextureType
{
	WINDOWS,
	SIDEWALK,
	SKY_BOX_TOP,
	SKY_BOX_SIDE,
	SKY_BOX_BOTTOM,
	NIGHT_BOX_TOP,
	NIGHT_BOX_SIDE,
	NIGHT_BOX_BOTTOM
	
};

class Texture
{
public:
	GLuint id;
	Texture();
	Texture(GLuint name, TextureType type, unsigned int w, unsigned int h);
private:
	unsigned int width, height;
	unsigned int unlit, dimlit, welllit;
	TextureType texType;
	GLubyte * image;
	GLubyte randR, randG, randB; // Color offsets that can be used as a sudo-filter
	void initRandomColors();
	bool skybox();
	void setGreyscale(int row, int col, int width, GLubyte color, bool filter);
	void setColor(int row, int col, int width, GLubyte r, GLubyte g, GLubyte b);
	void colorWindow(int xOffset, int yOffset);
	void setBrightnessIntervals();
	void constructWindows();
	void createSkyGradient();
	void createSkyStars();
	void createSkyBottom();
	void createNightGradient();
	void createNightStars();
	void createNightBottom();
	
	void pourSidewalk();
	void createGLTexture();
	GLubyte randomWindowColor();
	GLubyte randomSidewalkColor();
};

void Texture::setColor(int row, int col, int width, GLubyte r, GLubyte g, GLubyte b)
{
	//int offset = row * width * 4 + col * 4;
	int offset = row * width * 3 + col * 3;

	*(image + offset) = r;
	*(image + (++offset)) = g;
	*(image + (++offset)) = b;
	//*(image + (++offset)) = (GLubyte) 255;
}

void Texture::setGreyscale(int row, int col, int width, GLubyte color, bool filter)
{
	if(color == BRIGHT)
	{
		setColor(row, col, width,
			color + (filter? color - (BRIGHT - randR) : 0.0),
			color + (filter? color - (BRIGHT - randB) : 0.0),
			color + (filter? color - (BRIGHT - randG) : 0.0));
	}
	else
	{
		setColor(row, col, width,
			color + (filter? color + randR : 0.0),
			color + (filter? color + randB : 0.0),
			color + (filter? color + randG : 0.0));
	}
}

GLubyte Texture::randomWindowColor()
{
	unsigned int num = rand() % 100;
	if(num < unlit)
	{
		return (GLubyte) UNLIT;
	}
	else if (num < dimlit)
	{
		return (GLubyte) DIMLIT;
	}
	else if(num < welllit)
	{
		return (GLubyte) WELL_LIT;
	}
	else
	{
		return (GLubyte) BRIGHT;
	}
}

GLubyte Texture::randomSidewalkColor()
{
	return (GLubyte) (255 - (rand() % 100) - 20);
}

void Texture::initRandomColors()
{
	randR = (GLubyte) (rand() % COLOR_RANGE);
	randG = (GLubyte) (rand() % COLOR_RANGE);
	randB = (GLubyte) (rand() % COLOR_RANGE);
}

bool Texture::skybox()
{
	return texType == SKY_BOX_TOP || texType == SKY_BOX_SIDE || texType == SKY_BOX_BOTTOM;
}

void Texture::createGLTexture()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    // Texture to render to
	glGenTextures(1, &id);
	// Bind newly created tecture
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	// SHOULD I STILL DO MIPMAPPING ?
	// LOOKS GOOD FROM FAR BUT BLUR WHEN ZOOMED IN 
	if(MIPMAP && !skybox())
	{
		// repeat the pixels in the edge of the texture to 1px wide line at the edge
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		
		if(ANISOTROPIC)
		{
			GLfloat largest_supported_anisotropy; 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy); 
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,largest_supported_anisotropy); 
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height,
						GL_RGB, GL_UNSIGNED_BYTE, image);
	}
	else
	{
		// Poor filtering
		// if the texture is bigger than the image, get the avarege of the pixels next to it
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// if the texture is smaller than the image, get the avarege of the pixels next to it
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		
		// Give image to OpenGL
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, 
			height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}
}

void Texture::colorWindow(int xOffset, int yOffset)
{
	int xStart = xOffset * WINDOW_HEIGHT;
	int yStart = yOffset * WINDOW_WIDTH;
	int xPos, yPos;
	GLubyte color = randomWindowColor();
	GLubyte buildingCol = 0;
	GLubyte randMod;
	bool day = false;
	
	int randomCol = rand() % 100;
	if (randomCol < 50) {
		buildingCol = 15;
		day = true;
	}
		

	for(int row = 0; row < WINDOW_HEIGHT; row++)
	{
		for(int col = 0; col < WINDOW_WIDTH; col++)
		{
			xPos = xStart + row;
			yPos = yStart + col;
			if(row % WINDOW_HEIGHT < WINDOW_BEZEL)
			{
				setGreyscale(xPos, yPos, width, (GLubyte) buildingCol, day);
			}
			else if(row % WINDOW_HEIGHT >= WINDOW_HEIGHT - WINDOW_BEZEL)
			{
				setGreyscale(xPos, yPos, width, (GLubyte) buildingCol, day);
			}
			else
			{
				if(col % WINDOW_WIDTH < WINDOW_BEZEL)
				{
					setGreyscale(xPos, yPos, width, (GLubyte) buildingCol, day);
				}
				else if(col % WINDOW_WIDTH >= WINDOW_WIDTH - WINDOW_BEZEL)
				{
					setGreyscale(xPos, yPos, width, (GLubyte) buildingCol, day);
				}
				else
				{
					randMod = (GLubyte) ((color % row) / ((rand() % 4)+1))*color;
					if(color > randMod)
						setGreyscale(xPos, yPos, width, color-randMod, color != 245);
					else
						setGreyscale(xPos, yPos, width, color+randMod, color != 245);
				}
			} 
		}
	}
}

void Texture::pourSidewalk()
{
	for(unsigned int row = 0; row < height; row++)
	{
		for(unsigned int col = 0; col < width; col++)
		{

			setGreyscale(row, col, width, randomSidewalkColor(), false);
 
		}
	}
}

void Texture::createSkyGradient()
{
    int cloud = 997;
   
	for(unsigned int row = 0; row < height; row++)
	{
		for(unsigned int col = 0; col < width; col++)
		{
			if(row > (height/2))
			{
			    if((rand() % 1000) > cloud)
			    {
				    setColor(row, col, width, 240, 240, 240);
				    cloud = 557;
			    }
			    else
			    {
				    setColor(row, col, width, 108, 217, 255);
				    cloud = 997;
			    }   
				
			}
			else
			{
				setColor(row, col, width, (208-(row / 5)), (235-(row / 30)), 255);
			}
		}
	}
}

void Texture::createSkyBottom()
{

	for(unsigned int row = 0; row < height; row++)
	{
		for(unsigned int col = 0; col < width; col++)
		{
			setColor(row, col, width, 208, 235, 255);
		}
	}
}

void Texture::createSkyStars()
{
	for(unsigned int row = 0; row < height; row++)
	{
		for(unsigned int col = 0; col < width; col++)
		{
			setColor(row, col, width, 106, 216, 254);
		}
	}	
}

void Texture::createNightGradient()
{
	for(unsigned int row = 0; row < height; row++)
	{
		for(unsigned int col = 0; col < width; col++)
		{
			if(row > (height/2))
			{
				if((rand() % 1000) > 998)
				{
					setColor(row, col, width, 200, 200, 200);
				}
				else
				{
					setColor(row, col, width, 0, 3, 18);
				}
			}
			else
			{
				if(row > (height / 3) && (rand() % 10000) > 9995)
					setColor(row, col, width, 200, 200, 200);
				else if(row > (height / 4) && (rand() % 10000) > 9997)
					setColor(row, col, width, 200, 200, 200);
				else
					setColor(row, col, width, 0, 3, (64 - (row / 8))+18);
			}
		}
	}
}

void Texture::createNightBottom()
{
	for(unsigned int row = 0; row < height; row++)
	{
		for(unsigned int col = 0; col < width; col++)
		{
			setColor(row, col, width, 0, 3, 82);
		}
	}
}

void Texture::createNightStars()
{
	for(unsigned int row = 0; row < height; row++)
	{
		for(unsigned int col = 0; col < width; col++)
		{
			if((rand() % 1000) > 997)
			{
				setColor(row, col, width, 200, 200, 200);
			}
			else
			{
				setColor(row, col, width, 0, 3, 18);
			}
		}
	}
}




void Texture::setBrightnessIntervals()
{
	unlit = (rand() % 40) + (rand() % 10);
	dimlit = (rand() % 40) + (rand() % 10) + unlit;
	welllit = (rand() % 40) + (rand() % 10) + dimlit;
}

void Texture::constructWindows()
{
	setBrightnessIntervals();
	for(unsigned int row = 0; row < height / WINDOW_HEIGHT; row++)
	{
		for(unsigned int col = 0; col < width / WINDOW_WIDTH; col++)
		{
			colorWindow(row, col);
		}
	}
	
}

Texture::Texture(GLuint name, TextureType type, unsigned int w, unsigned int h)
{
	id = name;
	width = w;
	height = h;
	texType = type;
	initRandomColors();

	// allocate memory for uninitialised var image
	image = (GLubyte *)malloc(width * height * 3);
	switch(type)
	{
	case WINDOWS:
		constructWindows();
		break;
	case SIDEWALK:
		pourSidewalk();
		break;
	case SKY_BOX_TOP:
		createSkyStars();
		break;
	case SKY_BOX_SIDE:
		createSkyGradient();
		break;
	case SKY_BOX_BOTTOM:
		createSkyBottom();
		break;
    case NIGHT_BOX_TOP:
		createNightStars();
		break;
	case NIGHT_BOX_SIDE:
   		createNightGradient();
		break;
	case NIGHT_BOX_BOTTOM:
   		createNightBottom();
		break;
	}
	createGLTexture();
	free(image);
}

Texture::Texture()
{
}

#endif
