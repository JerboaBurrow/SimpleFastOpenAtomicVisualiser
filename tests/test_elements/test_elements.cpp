#include <colour.h>
#include <element.h>

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

SCENARIO("String to Element")
{
    GIVEN("The string C")
    {
        THEN("stringSymbolToElement(\"C\") is Element::C")
        {
            REQUIRE(stringSymbolToElement("C") == Element::C);
        }
    }
    GIVEN("The string Car")
    {
        THEN("stringSymbolToElement(\"Car\") is Element::C")
        {
            REQUIRE(stringSymbolToElement("Car") == Element::C);
        }
    }
    GIVEN("The string CAR")
    {
        THEN("stringSymbolToElement(\"CAR\") is Element::C")
        {
            REQUIRE(stringSymbolToElement("CAR") == Element::C);
        }
    }
    GIVEN("The string Calc")
    {
        THEN("stringSymbolToElement(\"Calc\") is Element::Ca")
        {
            REQUIRE(stringSymbolToElement("Calc") == Element::Ca);
        }
    }
    GIVEN("The string NLT")
    {
        THEN("stringSymbolToElement(\"NLT\") is Element::N")
        {
            REQUIRE(stringSymbolToElement("NLT") == Element::N);
        }
    }
    GIVEN("The string OW")
    {
        THEN("stringSymbolToElement(\"OW\") is Element::O")
        {
            REQUIRE(stringSymbolToElement("OW") == Element::O);
        }
    }
    GIVEN("The string OXYGEN")
    {
        THEN("stringSymbolToElement(\"OXYGEN\") is Element::O")
        {
            REQUIRE(stringSymbolToElement("OXYGEN") == Element::O);
        }
    }
    GIVEN("The string OXYGENHYDROGEN")
    {
        THEN("stringSymbolToElement(\"OXYGENHYDROGEN\") is Element::O")
        {
            REQUIRE(stringSymbolToElement("OXYGENHYDROGEN") == Element::O);
        }
    }
}
