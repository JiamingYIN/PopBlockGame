#include "GameScene3.h"

USING_NS_CC;

Scene* GameScene_3::createScene()
{
	auto scene = Scene::create();

	auto layer = GameScene_3::create();

	scene->addChild(layer);

	return scene;
}

bool GameScene_3::init()
{
	if (!Layer::init())
	{
		return false;
	}
	iCurrent = 3;
	auto rootNode = CSLoader::createNode("GameScene3.csb");

	root = (Layout*)rootNode->getChildByName("root");
	MusicControl = (cocos2d::ui::ImageView*)Helper::seekWidgetByName(root, "Image_5");
	ComeBackToBeginScene = (cocos2d::ui::Button*)Helper::seekWidgetByName(root, "Button_3");

	MusicControl->loadTexture("res/menu_sound_on.png");
	MusicControl->setTouchEnabled(true);
	MusicControl->addTouchEventListener(CC_CALLBACK_2(GameScene_3::MusicControling, this));


	ComeBackToBeginScene->addTouchEventListener(CC_CALLBACK_2(GameScene_3::ComeBack, this));
	addChild(rootNode);

	scoreLabel0 = Label::createWithTTF("Score:", "fonts/Marker Felt.ttf", 36);
	scoreLabel0->setColor(Color3B::BLACK);
	scoreLabel0->setPosition(Vec2(100, 450));
	addChild(scoreLabel0);

	scoreLabel = Label::createWithTTF("0", "fonts/Marker Felt.ttf", 36);
	scoreLabel->setColor(Color3B::BLACK);
	scoreLabel->setPosition(Vec2(200, 450));
	addChild(scoreLabel);
	//////////////////////////////////////////////////////////////////

	addMap();
	addBlock();

	auto listener = EventListenerTouchOneByOne::create();
	// 绑定监听事件
	listener->onTouchBegan = CC_CALLBACK_2(GameScene_3::onTouchBegan, this);
	listener->setSwallowTouches(true);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	/////////////////////////////////////////////////////////////////////////////////////////////////

	return true;
}

void GameScene_3::ComeBack(cocos2d::Ref*pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case Widget::TouchEventType::ENDED:
		auto director = Director::getInstance();
		auto scene = BeginScene::createScene();
		auto trasition = TransitionCrossFade::create(1.0f, scene);
		director->replaceScene(trasition);
		break;
	}
}

void GameScene_3::addMap()
{
	iScore = 0;
	srand(unsigned(time(0)));
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			map1[x][y] = rand() % 6 + 1;
			srand(rand()*rand());//更新随机数种子
			map2[x][y] = 10 * y + x;
			record[x][y] = -1;
		}
	}
	map1[3][7] = 8;
	map1[4][5] = 8;
	map1[7][9] = 8;
}

void GameScene_3::addBlock()
{
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			blocks[x][y] = new Block(map1[x][y], x, y);
			this->addChild(blocks[x][y]->block);
			auto move = MoveTo::create((640 - blocks[x][y]->posY * 60) / 200.0, Vec2(370 + 60 * blocks[x][y]->posX, 50 + 60 * blocks[x][y]->posY));
			blocks[x][y]->block->runAction(move);
		}
	}
	NumOfBlocks = 0;
}

bool GameScene_3::onTouchBegan(Touch* touch, Event* event)
{
	auto position = touch->getLocation();   //获取用户点击坐标
	//判断用户点击的坐标位置是否在“星星”的范围内
	if (position.x<940 && position.x>340 && position.y > 20 && position.y < 620)
	{
		//将坐标转换为对应在blocks数组的索引
		int x = (int)(position.x - 340) / 60;
		int y = (int)(position.y - 20) / 60;
		
		int posx = map2[x][y] % 10;
		int posy = map2[x][y] / 10;
		int ix, iy;
		if (map1[x][y]>0&&blocks[posx][posy]->type == 8)
		{
			iScore += 250;
			map1[x][y] = 0;
			map2[x][y] = -1;
			sprintf(score_str, "%d", iScore);
			blocks[posx][posy]->blockDelete();
			if (x > 0&&map1[x-1][y]>0)
			{
				CCLOG("%d", map2[x - 1][y]);
				ix = map2[x-1][y] % 10;
				iy = map2[x-1][y] / 10;
				CCLOG("%d%d", ix, iy);
				blocks[ix][iy]->blockDelete();
				map1[x-1][y] = 0;
				map2[x-1][y] = -1;
			}
			if (x <9&&map1[x+1][y]>0)
			{
				ix = map2[x + 1][y] % 10;
			    iy = map2[x + 1][y] / 10;
				blocks[ix][iy]->blockDelete();
				CCLOG("%d%d", ix, iy);
				map1[x + 1][y] = 0;
				map2[x + 1][y] = -1;
			}
			if (y > 0&&map1[x][y-1]>0)
			{
				ix = map2[x][y-1] % 10;
				iy = map2[x][y-1] / 10;
				blocks[ix][iy]->blockDelete();
				CCLOG("%d%d", ix, iy);
				map1[x][y-1] = 0;
				map2[x][y-1] = -1;
			}
			if (y<9&&map1[x][y+1]>0)
			{
				ix = map2[x][y+1]%10;
				iy = map2[x][y+1] / 10;
				blocks[ix][iy]->blockDelete();
				CCLOG("%d%d", ix, iy);
				map1[x][y+1] = 0;
				map2[x][y+1] = -1;
			}

			scoreLabel->setString(score_str);
			this->blockDrop();
			this->moveLeft();
			return true;
		}
		else{
			if (canDelete(x, y) == true)
			{
				SimpleAudioEngine::getInstance()->playEffect("res/music/pop.mp3");
				UDLRDelete(x, y);
				//后续操作：
				iScore += NumOfBlocks*NumOfBlocks * 10;
				CCLOG("%d", iScore);
				this->blockDrop();
				this->moveLeft();
				this->cleanRecord();
				NumOfBlocks = 0;

				auto userdefault = UserDefault::getInstance();

				sprintf(score_str, "%d", iScore);

				scoreLabel->setString(score_str);


				if (isOver() == true)
				{
					if (iScore >= 3000)
					{
						Clear();
						auto layer = YouwinLayer::create();
						this->addChild(layer);
					}
					else
					{
						Clear();
						auto layer = GameOverLayer::create();
						this->addChild(layer);
					}
				}
				return true;
			}
		}
	}
	return false;
}

bool GameScene_3::canDelete(int x, int y)
{
	int type = map1[x][y];
	bool isadd = false;
	if (map1[x][y] > 0&&map1[x][y]<6)
	{
		if (y + 1 < 10 && map1[x][y + 1] == type&&record[x][y + 1] == -1)//上方存在未检测过的同色方块
		{
			//Up
			isadd = true;
		}
		if (y > 0 && map1[x][y - 1] == type&&record[x][y - 1] == -1)
		{
			//Down
			isadd = true;
		}
		if (x > 0 && map1[x - 1][y] == type&&record[x - 1][y] == -1)
		{
			//Left
			isadd = true;
		}
		if (x + 1 < 10 && map1[x + 1][y] == type&&record[x + 1][y] == -1)
		{
			//Right
			isadd = true;
		}
		if (isadd == true)
		{
			record[x][y] = 0;
			NumOfBlocks++;
			map1[x][y] = 0;
			CurrentType = type;
			return true;
		}
		else return false;
	}
	else return false;
}

void GameScene_3::UDLRDelete(int x, int y)
{
	int posx = map2[x][y] % 10;
	int posy = map2[x][y] / 10;
	blocks[posx][posy]->blockDelete();//直接消除对应的方块
	map2[x][y] = -1;
	//依次计算对应点的上下左右相邻的星星
	if (y + 1 < 10 && map1[x][y + 1] == CurrentType&&record[x][y + 1] == -1)//上方存在未检测过的同色方块
	{
		//Up
		record[x][y + 1] = 0;
		map1[x][y + 1] = 0;
		NumOfBlocks++;
		UDLRDelete(x, y + 1);
	}
	if (y > 0 && map1[x][y - 1] == CurrentType&&record[x][y - 1] == -1)
	{
		//Down
		record[x][y - 1] = 0;
		map1[x][y - 1] = 0;
		NumOfBlocks++;
		UDLRDelete(x, y - 1);
	}
	if (x > 0 && map1[x - 1][y] == CurrentType&&record[x - 1][y] == -1)
	{
		//Left
		record[x - 1][y] = 0;
		map1[x - 1][y] = 0;
		NumOfBlocks++;
		UDLRDelete(x - 1, y);
	}
	if (x + 1 < 10 && map1[x + 1][y] == CurrentType&&record[x + 1][y] == -1)
	{
		//Right
		record[x + 1][y] = 0;
		map1[x + 1][y] = 0;
		NumOfBlocks++;
		UDLRDelete(x + 1, y);
	}
}

void GameScene_3::blockDrop()
{
	for (int y = 1; y < 10; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			// 遍历每一个非最底层的星星，看其下方是否存在空白

			if (map1[x][y]>0)//有方块，需要检测其是否能够向下移动
			{
				for (int i = 0; i < y; i++)
				{
					if (map1[x][i] == 0)//有空位
					{
						int posx = map2[x][y] % 10;
						int posy = map2[x][y] / 10;
						map1[x][i] = map1[x][y];
						map1[x][y] = 0;
						map2[x][i] = map2[x][y];
						map2[x][y] = -1;
						blocks[posx][posy]->moveTo(x, i);
						break;
					}
				}
			}
		}
	}
}

void GameScene_3::moveLeft()
{
	for (int x = 1; x < 10; x++)
	{
		if (map1[x][0] >0)
		{
			for (int i = 0; i < x; i++)
			{
				if (map1[i][0] == 0)
				{
					for (int y = 0; y < 10; y++)
					{
						if (map1[x][y]>0)
						{
							int posx = map2[x][y] % 10;
							int posy = map2[x][y] / 10;
							map1[i][y] = map1[x][y];
							map1[x][y] = 0;
							map2[i][y] = map2[x][y];
							map2[x][y] = -1;
							blocks[posx][posy]->moveTo(i, y);
						}
					}
					break;
				}
			}

		}
	}
}

void GameScene_3::cleanRecord()
{
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
			record[x][y] = -1;
	}
}


bool GameScene_3::isOver()
{
	bool result = true;
	for (int x = 0; x < 9; x++)
	{
		for (int y = 0; y < 9; y++)
		{
			if ((map1[x][y] != 0) && map1[x + 1][y] == map1[x][y]&&map1[x][y]!=6)
				result = false;
			if (map1[x][y] != 0 && map1[x][y] == map1[x][y + 1]&&map1[x][y]!=6)
				result = false;
		}
	}
	return result;
}

void GameScene_3::Clear()
{
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			blocks[i][j]->block->removeFromParent();
		}
	}
}

void GameScene_3::MusicControling(cocos2d::Ref*pSender, Widget::TouchEventType type)
{

	switch (type)
	{
	case Widget::TouchEventType::ENDED:

		if (iMusic == 1)
		{
			iMusic = 0;
			MusicControl->loadTexture("menu_sound_off.png");
			SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
		}
		else
		{
			iMusic = 1;
			MusicControl->loadTexture("menu_sound_on.png");
			SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
		}
		break;
	}
}

