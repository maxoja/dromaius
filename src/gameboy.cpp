#include "gameboy.h"
#include "spdlog/spdlog.h"
#include <SFML/Graphics.hpp>
#include <chrono>
#include "memorylocs.h"
#include <iostream>

Gameboy::Gameboy(std::string cartridgeName) :
	memoryMap(cartridgeName), cpu(memoryMap), gpu(memoryMap), timer(memoryMap)
{

	sf::RenderWindow window(sf::VideoMode(256,256), "Dromaius");
	sf::View view;
	view.setSize(sf::Vector2f(160, 144));

	spdlog::get("console")->info("Powering up Gameboy");
	Ticks tickTotal = 0;
	//auto begin = std::chrono::high_resolution_clock::now();
    while (1)
    {
		auto ticks = cpu.process();
		tickTotal += ticks;
		gpu.process(ticks);
		timer.process(ticks);
		
		//TODO: Change the location of this, maybe make a callback function for gpu to call
		//This is each frame (~60Hz) 
		if (tickTotal > (456*154)){
			

			// #include <SFML/Window/Keyboard.hpp>
			// auto pause = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
			// if (pause){
			// 	cpu.ping();
			// 	std::cin.get();
			// }
			// auto end = std::chrono::high_resolution_clock::now();
			// std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << "us" << std::endl;
			// begin = std::chrono::high_resolution_clock::now();

			tickTotal = 0;
			gpu.initialiseTileMapData();
			gpu.renderBackground();
			gpu.renderSprites();
			auto scx = memoryMap.byte(SCX).val();
			auto scy = memoryMap.byte(SCY).val();
			view.setCenter(scx+80,scy+72);
			window.clear();
			window.setView(view);
			for (auto tile : gpu.backgroundTiles){
				window.draw(tile);
			}
			for (auto sprite : gpu.sprites){
				window.draw(sprite);
			}
			window.display();

		}
    }  
	
}