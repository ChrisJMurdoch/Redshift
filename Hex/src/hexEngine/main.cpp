
#include "render/display.hpp"
#include "model/instance.hpp"
#include "hexEngine/hexCell.hpp"
#include "arrayBuffer/vertexBuffer.hpp"
#include "generate/noise.hpp"

#include <iostream>
#include <exception>
#include <random>

static std::random_device device;
static std::mt19937 twister(device());
static std::uniform_real_distribution<float> uniform(1.0f, 1.3f);

int main()
{
    try
    {
        // Create display and opengl context
        Display display(1000, 600, "WorldEngine");

        // Create programs
        Program post("shaders\\post.vert", "shaders\\post.frag");
        Program hex("shaders\\hex.vert", "shaders\\hex.frag");

        // Add post-processing to display
        display.addPostProgram(&post);

        // Create instances
        int const gridWidth = 500, gridDepth = 500;
        Pos instances(gridWidth*gridDepth);
        float const xOrigin = -(gridWidth-1)/2.0f, zOrigin = -(gridDepth-1)/2.0f;
        std::cout << xOrigin << ", " << zOrigin << std::endl << std::endl;
        for (int z=0; z<gridDepth; z++)
        {
            for (int x=0; x<gridWidth; x++)
            {
                int const index = (z*gridWidth)+x;

                // Procedural noise
                float const noise = noise::fractalSample(x, 0, z, 10.0f, 10, 0);

                // Coords
                float xVal = (xOrigin+x)*1.5f*HexCell::RADIUS;
                float zVal = (x%2==0) ? (zOrigin+z)*2*HexCell::APOTHEM : (zOrigin+z+0.5f)*2*HexCell::APOTHEM;
                float yVal;
                static float const WATER_DEV = 0.03f;
                if (noise < -WATER_DEV)
                {
                    yVal = 0.0f; // Water
                }
                else if (noise < WATER_DEV)
                {
                    yVal = 0.1f; // Grass
                }
                else
                {
                    yVal = 0.2f; // Sand
                }

                // Gap
                xVal *= 1.00f, zVal *= 1.00f;

                // Set
                instances.position(index)->xyz(xVal, yVal, zVal);
            }
        }

        // Create hex cell
        HexCell cell = HexCell();
        Model hexModel(&cell.getVertices(), &cell.getIndices(), &instances, &hex);

        // Add instances
        Instance hexInstance = Instance(hexModel);
        display.addInstance(&hexInstance);

        // Run display
        while (!display.shouldClose())
        {
            hexInstance.rotate(glm::vec3(0.0f, 1.0f, 0.0f), 0.01f);
            display.render();
        }

        return 0;
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return -1;
    }
}