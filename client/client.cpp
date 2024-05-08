#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>

#include "view.h"//���������� ��� � ����� ������
#include "map.h" //���������� ��� � ������

#include "NetworkClient.h"

using namespace sf;
using namespace std;


class Player
{
private:
	float x, y;
	Text t;
	Sprite body, netGhost;
	bool possesed = false;
public:
	//float w, h, dx, dy, speed; //���������� ������ � � �, ������ ������, ��������� (�� � � �� �), ���� ��������
	//int dir; //����������� (direction) �������� ������
	//String File; //���� � �����������
	//Image image;//���� �����������
	//Texture texture;//���� ��������
	//Sprite sprite;//���� ������
	string name;
	enum Direction { Left, Right, Up, Down } direction = Right;


	Player(bool possesed = false) :possesed(possesed) {
	};
	void load(Texture& texture, Font& font)
	{
		body.setTexture(texture);
		body.setTextureRect(IntRect(102, 0, 100, 100));
		if (!possesed) body.setColor(Color::Red);
		netGhost.setTexture(texture);

		t.setFont(font);
		t.setString(name);
		t.setFillColor(sf::Color::Red);
		t.setPosition(body.getGlobalBounds().width / 2 - t.getGlobalBounds().width / 2, body.getPosition().y - t.getGlobalBounds().height);
	};
	void setPosition(Vector2f newPos)
	{
		body.setPosition(newPos);
		t.setPosition(newPos.x + body.getGlobalBounds().width / 2 - t.getGlobalBounds().width / 2, body.getPosition().y - t.getGlobalBounds().height);
	};
	void move(Vector2f normalizedMovementVec, Time cycleTime)
	{

		body.move({ normalizedMovementVec.x * 50 * cycleTime.asSeconds(), normalizedMovementVec.y * 50 * cycleTime.asSeconds() });
		t.move({ normalizedMovementVec.x * 50 * cycleTime.asSeconds(), normalizedMovementVec.y * 50 * cycleTime.asSeconds() });

		// ��������� ����������� � �������� ��� �������� �����
		if (normalizedMovementVec.x < 0)
		{
			direction = Left;
			body.setTextureRect(IntRect(102, 110, 100, 100)); // ���������� ��� ��������, ����� �������� ������� �����
		}
		if (normalizedMovementVec.x > 0)
		{
			direction = Right;
			body.setTextureRect(IntRect(102, 0, 100, 100)); // ���������� ��� ��������, ����� �������� ������� �����
		}
	};

	//�� ���������� �������
	//void update(float time) //������� "���������" ������� ������. update - ����������. ��������� � ���� ����� SFML , ���������� ���� �������� ����������, ����� ��������� ��������.
	//{
	//	switch (dir)//��������� ��������� � ����������� �� �����������. (������ ����� ������������� �����������)
	//	{
	//	case 0: dx = speed; dy = 0;   break;//�� ���� ������ ������������� ��������, �� ������ ��������. ��������, ��� �������� ���� ������ ������
	//	case 1: dx = -speed; dy = 0;   break;//�� ���� ������ ������������� ��������, �� ������ ��������. ����������, ��� �������� ���� ������ �����
	//	case 2: dx = 0; dy = speed;   break;//�� ���� ������ ������� ��������, �� ������ �������������. ����������, ��� �������� ���� ������ ����
	//	case 3: dx = 0; dy = -speed;   break;//�� ���� ������ ������� ��������, �� ������ �������������. ����������, ��� �������� ���� ������ �����
	//	}

	//	x += dx * time;//�� �������� �� �������� �����. ���� ��������� �� ����� �������� �������� ��������� � ��� ��������� ��������
	//	y += dy * time;//���������� �� ������

	//	speed = 0;//�������� ��������, ����� �������� �����������.
	//	sprite.setPosition(x, y); //������� ������ � ������� x y , ����������. ���������� ������� � ���� �������, ����� �� ��� ������ ����� �� �����.
	//	//interactionWithMap();//�������� �������, ���������� �� �������������� � ������
	//}

	void draw(RenderWindow& window)
	{
		//window.draw(netGhost);
		window.draw(body);
		window.draw(t);
	};
	float getplayercoordinateX() {	//���� ������� ����� �������� ���������� �	
		return x;
	}
	float getplayercoordinateY() {	//���� ������� ����� �������� ���������� Y 	
		return y;
	}

	bool isPossesed() { return possesed; };
	Vector2f getPos() { return body.getPosition(); };
};

vector<Player> playersVec;

Clock cycleTimer;
Time cycleTime;

IpAddress S_Ip;
unsigned short S_port;
string clientName;

NetworkClient netC;

Player player(true);

void getUserInputData(string& playerName);
void addPlayer(Texture& t_player, Font& font, string clientName);

int main()
{
	RenderWindow window(sf::VideoMode(800, 800), "SFML!");
	sf::View view(sf::FloatRect(0, 0, 800, 800));


	Image map_image;//������ ����������� ��� �����
	map_image.loadFromFile("map.png");//��������� ���� ��� �����
	Texture map;//�������� �����
	map.loadFromImage(map_image);//�������� �������� ���������
	Sprite s_map;//������ ������ ��� �����
	s_map.setTexture(map);//�������� �������� ��������

	Texture t_player;
	t_player.loadFromFile("vor1.png");
	Font font;
	font.loadFromFile("8bitOperatorPlus-Regular.ttf");

	Clock clock; //������� ���������� �������, �.�. �������� �� �������(� �� �������������/�������� ����������).

	getUserInputData(player.name);
	player.load(t_player, font);




	netC.init();
	netC.registerOnServer(S_Ip, S_port, player.name);

	vector<string> namesVec;
	netC.receiveConnectedClientsNames(namesVec);
	for (int i = 0; i < namesVec.size(); i++)
	{
		addPlayer(t_player, font, namesVec[i]);
	}

	Packet receivedDataPacket;
	Packet sendDataPacket;



	while (window.isOpen())
	{
		float currentZoom = 1.0f;
		float time = clock.getElapsedTime().asMicroseconds(); //���� ��������� ����� � �������������
		clock.restart(); //������������� �����
		time = time / 800; //�������� ����
		cycleTime = cycleTimer.restart();

		if (netC.receiveData(receivedDataPacket, S_Ip, S_port) == Socket::Status::Done)
		{
			if (receivedDataPacket.getDataSize() > 0)
			{
				string s;
				if (receivedDataPacket >> s)
				{
					if (s == "NEW")
					{
						if (receivedDataPacket >> s)
						{
							if (s != clientName)
							{
								addPlayer(t_player, font, s);
								cout << "New player connected: " << playersVec.back().name << endl;
							}
						}
					}
					if (s == "DATA")
					{
						while (!receivedDataPacket.endOfPacket())
						{
							float x, y;
							receivedDataPacket >> s;
							receivedDataPacket >> x;
							receivedDataPacket >> y;
							for (int i = 0; i < playersVec.size(); i++)
							{
								if (s == playersVec[i].name)
									playersVec[i].setPosition({ x, y });
							}
						}
					}
				}
			}
		}

		sendDataPacket.clear();
		sendDataPacket << "DATA" << player.getPos().x << player.getPos().y;
		netC.sendData(sendDataPacket);




		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed)
			{
				switch (event.key.code)
				{
				case sf::Keyboard::U:
					currentZoom *= 0.9f; // ��������� �������
					view.zoom(0.9f);
					break;
				case sf::Keyboard::R:
					currentZoom *= 1.1f; // ����������� �������
					view.zoom(1.1f);
					break;
					// ... ��������� ������ ������ ...
				case sf::Keyboard::N:
					view.setSize(800, 800);
					break;
					// ... ��������� ������ ������ ...
				}

			}
			// ��������� ������� ������ ��� ���������� �������
			if (event.type == sf::Event::KeyPressed)
			{
				switch (event.key.code)
				{
				case sf::Keyboard::Left:
					view.move(-10, 0); // ����� ������ �����
					break;
				case sf::Keyboard::Right:
					view.move(10, 0); // ����� ������ ������
					break;
				case sf::Keyboard::Up:
					view.move(0, -10); // ����� ������ �����
					break;
				case sf::Keyboard::Down:
					view.move(0, 10); // ����� ������ ����
					break;
				default:
					break;
				}
			}
		}

		if (window.hasFocus())
		{
			if (Keyboard::isKeyPressed(Keyboard::W)) {
				player.move({ 0, -1 }, cycleTime);
				updateCameraPosition(player.getPos(), view, player.getPos().x, player.getPos().y);
			}
			if (Keyboard::isKeyPressed(Keyboard::A)) {
				player.move({ -1, 0 }, cycleTime);
				updateCameraPosition(player.getPos(), view, player.getPos().x, player.getPos().y);
			}

			if (Keyboard::isKeyPressed(Keyboard::S)) {
				player.move({ 0, 1 }, cycleTime);
				updateCameraPosition(player.getPos(), view, player.getPos().x, player.getPos().y);
			}
			if (Keyboard::isKeyPressed(Keyboard::D)) {
				player.move({ 1, 0 }, cycleTime);
				updateCameraPosition(player.getPos(), view, player.getPos().x, player.getPos().y);
			}
		}
		//player.update(time);
		window.setView(view);
		window.clear();
		for (int i = 0; i < HEIGHT_MAP; i++) {
			for (int j = 0; j < WIDTH_MAP; j++)
			{
				if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 0, 32, 32)); //���� ��������� ������ ������, �� ������ 1� ���������
				if (TileMap[i][j] == 's')  s_map.setTextureRect(IntRect(32, 0, 32, 32));//���� ��������� ������ s, �� ������ 2� ���������
				if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(64, 0, 32, 32));//���� ��������� ������ 0, �� ������ 3� ���������


				s_map.setPosition(j * 32, i * 32);//����������� ����������, ��������� � �����. �� ���� ������ ������� �� ��� �������. ���� ������, �� ��� ����� ���������� � ����� �������� 32*32 � �� ������ ���� �������

				window.draw(s_map);//������ ���������� �� �����
			}
		}
		for (int i = 0; i < playersVec.size(); i++)
		{
			playersVec[i].draw(window);
		}

		player.draw(window);

		window.display();
	}

	return 0;
};

void getUserInputData(string& playerName)
{
	S_Ip = "localhost";
	cout << endl;
	cout << "Enter server registration port: ";
	cin >> S_port;
	cout << endl;
	cout << "Enter name: ";
	cin >> playerName;
};

void addPlayer(Texture& t_player, Font& font, string clientName)
{
	Player p;
	playersVec.push_back(p);
	playersVec.back().name = clientName;
	playersVec.back().load(t_player, font);
};