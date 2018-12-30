#include "memorymap.h"
#include "memorylocs.h"
#include "bytes/byte.h"
#include <SFML/Window/Keyboard.hpp>
#include <fstream>
#include <iostream>

MemoryMap::MemoryMap(std::string cartridgeName) {
	//Load bootRom
	std::ifstream bootRomStream {"roms/dmg_boot.bin"};
	if (bootRomStream.good()){
		bootRomStream.seekg(0, bootRomStream.end);
		bootRomSize = bootRomStream.tellg();
		spdlog::get("console")->info("LOADING Boot Rom - Size: {}",bootRomSize);
		bootRomStream.seekg(0, bootRomStream.beg);
		bootRomStream.read(reinterpret_cast<char*>(bootRom.data()), bootRomSize);
	}
	else {
		spdlog::get("stderr")->error("Couldnt open boot rom");
	}

	//Load cartridge
	std::ifstream cartridgeStream {cartridgeName};
	if (cartridgeStream.good()){
		cartridgeStream.seekg(0, cartridgeStream.end);
		cartridgeSize = cartridgeStream.tellg();
		spdlog::get("console")->info("LOADING Cartridge - Size: {}",cartridgeSize);
		cartridgeStream.seekg(0, cartridgeStream.beg);
		cartridgeStream.read(reinterpret_cast<char*>(cartridge.data()), cartridgeSize);
	}
	else {
		spdlog::get("stderr")->error("Couldnt open cartridge");
	}		
}


//Once disabled it cannot be re-enabled
bool MemoryMap::bootRomEnabled(){
	_bootRomEnabled = _bootRomEnabled && cartridge[BRD] != 1;
	return _bootRomEnabled;
}

Byte MemoryMap::byte(unsigned int address) {

	//TODO: Get rid, this should not belong here
	//Override Joypad register, always reports no buttons pressed
	if (address == P1){
		auto p1Byte = Byte(&cartridge[P1]);
		auto right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
		auto left = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
		auto up = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
		auto down = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);

		auto a = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
		auto b = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
		auto select = sf::Keyboard::isKeyPressed(sf::Keyboard::Num2);
		auto start = sf::Keyboard::isKeyPressed(sf::Keyboard::Num1);

		auto directionSwitch = !p1Byte.getBit(4);
		auto buttonSwitch = !p1Byte.getBit(5);


		auto p10 = (directionSwitch && right) 	|| (buttonSwitch && a);
		auto p11 = (directionSwitch && left) 	|| (buttonSwitch && b);
		auto p12 = (directionSwitch && up) 		|| (buttonSwitch && select);
		auto p13 = (directionSwitch && down) 	|| (buttonSwitch && start);
		
		p1Byte.setBit(0,	!p10);
		p1Byte.setBit(1,	!p11);
		p1Byte.setBit(2,	!p12);
		p1Byte.setBit(3,	!p13);
		

		// 			P14 		P15
		//  		| 			|
		//  P10-----O-Right-----O-A
		//  		| 			|
		//  P11-----O-Left------O-B
		//  		| 			|
		//  P12-----O-Up--------O-Select
		//  		| 			|
		//  P13-----O-Down------O-Start
		//http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf

		return p1Byte;
	}

	if (address == SC) {
		std::cout << byte(SB).val();
	}
	if (address == DIV){
		return Byte(&bootRom[address], ByteType::WRITE_RESET);
	}

	if (bootRomEnabled() && address < bootRomSize){
		return Byte(&bootRom[address], ByteType::NO_WRITE);
	}
	else{
		return Byte(&cartridge[address]);
	}
}