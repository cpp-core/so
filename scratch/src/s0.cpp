// Copyright (C) 2022, 2023 by Mark Melton
//

#include "core/util/tool.h"
#include "nlohmann/json.hpp"

struct RoomData {
    unsigned int id;
    std::string name;
    unsigned int maxPlayers;
    unsigned int numOfQuestionsInGame;
    unsigned int timePerQuestion;
    unsigned int isActive;
};

struct GetRoomsResponse {
    unsigned int status;
    std::vector<RoomData> rooms;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RoomData, id, name, maxPlayers, numOfQuestionsInGame, timePerQuestion, isActive);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GetRoomsResponse, status, rooms);

int tool_main(int argc, const char *argv[]) {

    RoomData r0{1, "abc", 2, 3, 4, 5}, r1{6, "def", 7, 8, 9, 10};
    GetRoomsResponse resp{0, {r0, r1}};

    nlohmann::json j{resp};
    cout << j << endl;
    return 0;
}
