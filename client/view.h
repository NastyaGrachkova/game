#include <SFML/Graphics.hpp>
using namespace sf;

sf::View view;//�������� sfml ������ "���", ������� � �������� �������
void updateCameraPosition(const sf::Vector2f& playerPosition, sf::View& view, float x, float y) {
	//view.setCenter(playerPosition);
	view.setCenter(x + 300, y + 300);
}
