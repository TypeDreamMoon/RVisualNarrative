#pragma once

class FRVNPlayToolBar final
{
public:
	FRVNPlayToolBar();

public:
	void Initialize();

	void Deinitialize();

private:
	void BuildAction();

	TSharedRef<SWidget> GeneratePlayToolBarMenu();

private:
	TSharedRef<FUICommandList> CommandList;
};
