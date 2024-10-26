#include <iostream>
#include <fstream>
#include <IO/System/CommandParser.hpp>
#include <IO/System/PrintDebug.hpp>
#include <IO/Commands/CreateMap.hpp>
#include <IO/Commands/SpawnWarrior.hpp>
#include <IO/Commands/SpawnArcher.hpp>
#include <IO/Commands/March.hpp>
#include <IO/System/EventLog.hpp>
#include <IO/Events/MapCreated.hpp>
#include <IO/Events/UnitSpawned.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitAttacked.hpp>

#include "Core/GameObject.hpp"
#include "Core/Game.hpp"
#include "Game/GameComponents.hpp"
#include "Game/Map.hpp"
#include "Game/Units.hpp"


int main(int argc, char** argv)
{
	using namespace sw;

	if (argc != 2) {
		throw std::runtime_error("Error: No file specified in command line argument");
	}

	std::ifstream file(argv[1]);
	if (!file) {
		throw std::runtime_error("Error: File not found - " + std::string(argv[1]));
	}

	// Code for example...

	std::cout << "Commands:\n";
	io::CommandParser parser;
	parser.add<io::CreateMap>(
		[](auto command)
		{
			printDebug(std::cout, command);
			CreateMap(command.width, command.height);
		}).add<io::SpawnWarrior>(
		[](auto command)
		{
			printDebug(std::cout, command);
			SpawnWarrior(command.unitId, command.x, command.y, command.hp, command.strength);
		}).add<io::SpawnArcher>(
		[](auto command)
		{
			printDebug(std::cout, command);
			SpawnArcher(command.unitId, command.x, command.y, command.hp, command.agility, command.strength, command.range);
		}).add<io::March>(
		[](auto command)
		{
			printDebug(std::cout, command);
			int id = command.unitId;
			GameObject::GetById(id)->GetComponent<Movable>()->March(command.targetX, command.targetY);
		});

	parser.parse(file);

	std::cout << "\n\nEvents:\n";

	Game game;
	while (game.Update()) {
		if (!Movable::WantSomeoneMove()) {
			break;
		}
		if (!Damagable::IsSomeoneAlive()) {
			break;
		}
	}

	return 0;
}
