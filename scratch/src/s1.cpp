
#include <iostream>
#include <vector>

class Subject
{
private:
    std::string name;
    std::vector<int> sectionPercentages;

public:
    Subject()
    {
        std::cout << "Write the name of the subject: ";
        std::cin >> this -> name;
        int percentage;
        
        while(true){
            std::cout << "Write a new section name and its percentage with no symbols: ";
            std::cin >> percentage;
            this -> sectionPercentages.push_back(percentage);
        }
    }
};

int main()
{
    std::vector<Subject> subjects;

    while(true)
    {
        Subject newSubject{};
        subjects.push_back(newSubject);
    }
    
    return 0;
}
