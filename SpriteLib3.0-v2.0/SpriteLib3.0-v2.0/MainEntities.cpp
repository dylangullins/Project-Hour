#include "MainEntities.h"

int MainEntities::m_mainCamera = 0;
int MainEntities::m_mainPlayer = 0;
int MainEntities::m_mainUI = 0;
int MainEntities::m_backgroundUI = 0;
int MainEntities::m_healthUI = 0;
int MainEntities::m_ammoUI = 0;
int MainEntities::m_tutorial = 0;

int MainEntities::MainCamera()
{
	return m_mainCamera;
}

int MainEntities::MainPlayer()
{
	return m_mainPlayer;
}

int MainEntities::MainUI()
{
	return m_mainUI;
}

int MainEntities::BackgroundUI()
{
	return m_backgroundUI;
}

int MainEntities::HealthUI()
{
	return m_healthUI;
}

int MainEntities::AmmoUI()
{
	return m_ammoUI;
}

int MainEntities::Tutorial()
{
	return m_tutorial;
}

void MainEntities::MainCamera(int main)
{
	m_mainCamera = main;
}

void MainEntities::MainPlayer(int main)
{
	m_mainPlayer = main;
}

void MainEntities::MainUI(int main)
{
	m_mainUI = main;
}

void MainEntities::BackgroundUI(int main)
{
	m_backgroundUI = main;
}

void MainEntities::HealthUI(int main)
{
	m_healthUI = main;
}

void MainEntities::AmmoUI(int main)
{
	m_ammoUI = main;
}

void MainEntities::Tutorial(int main)
{
	m_tutorial = main;
}
