#include <colour.h>

void checkVec4(glm::vec4 actual, glm::vec4 exected, double tol);
std::string randomFileName();

SCENARIO("Colourmap reading")
{
    GIVEN("The colourmap file CPK")
    {
        WHEN("It is read by coloursFromFile")
        {
            std::map<Element, glm::vec4> cmap = coloursFromFile("CPK");
            THEN("The colours are all equal to CPK_COLOURS")
            {
                for (const auto & ec : cmap)
                {
                    checkVec4(ec.second, CPK_COLOURS.at(ec.first));
                }
            }
        }
    }
    GIVEN("A colour map file that does not exist")
    {
        std::string file = randomFileName();
        std::cout << file << "\n";
        WHEN("It is read by coloursFromFile")
        {
            std::map<Element, glm::vec4> cmap = coloursFromFile(file);
            THEN("The colours are all equal to CPK_COLOURS")
            {
                for (const auto & ec : cmap)
                {
                    checkVec4(ec.second, CPK_COLOURS.at(ec.first));
                }
            }
        }
    }
}
