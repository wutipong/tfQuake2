#include <stdio.h>

#include <IApp.h>
#include <ILog.h>

class MainApp : public IApp
{
public:
    bool Init() override;
    void Exit() override;
    bool Load(ReloadDesc *pReloadDesc) override;
    void Unload(ReloadDesc *pReloadDesc) override;
    void Draw() override;
    void Update(float deltaTime) override;
    const char *GetName() override;
};

const char *MainApp::GetName() { return "tfQuake2"; }

bool MainApp::Init()
{ 
    LOGF(LogLevel::eDEBUG, "Hello World");
    return true;
}

void MainApp::Exit()
{
    LOGF(LogLevel::eDEBUG, "Good bye!");
}

bool MainApp::Load(ReloadDesc *pReloadDesc)
{
    return true;
}

void MainApp::Unload(ReloadDesc *pReloadDesc)
{
}

void MainApp::Update(float deltaTime)
{
    mSettings.mQuit = true;
}

void MainApp::Draw()
{
}

DEFINE_APPLICATION_MAIN(MainApp);