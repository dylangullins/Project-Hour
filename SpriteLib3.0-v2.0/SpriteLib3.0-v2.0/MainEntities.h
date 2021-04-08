#ifndef __MAINENTITIES_H__
#define __MAINENTITIES_H__

class MainEntities
{
public:
	//Getters
	static int MainCamera();
	static int MainPlayer();
	static int MainUI();
	static int BackgroundUI();
	static int HealthUI();
	static int AmmoUI();
	static int Tutorial();

	//Setters
	static void MainCamera(int main);
	static void MainPlayer(int main);
	static void MainUI(int main);
	static void BackgroundUI(int main);
	static void HealthUI(int main);
	static void AmmoUI(int main);
	static void Tutorial(int main);

private:
	static int m_mainCamera;
	static int m_mainPlayer;
	static int m_mainUI;
	static int m_backgroundUI;
	static int m_healthUI;
	static int m_ammoUI;
	static int m_tutorial;
};

#endif // !__MAINENTITIES_H__
