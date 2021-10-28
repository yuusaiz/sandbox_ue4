// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

#include "Widgets/SWindow.h"
#include "Framework/Application/SlateApplication.h"
#include "TimerManager.h"
#include "Engine/GameViewportClient.h"
//#include "SWebBrowser.h"
//#include "MyObject.h"

#define LOCTEXT_NAMESPACE "MyGameInstance" 

void UMyGameInstance::InitSlate()
{
	//GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]()
	{
		TSharedRef<SWindow> window = SNew(SWindow)
			.Title(LOCTEXT("CreateWindow", "Test Slate Window"))
			.ClientSize(FVector2D(512.0f, 512.0f))
			[
				SNew(SBox)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
					.Text(LOCTEXT("CreateButton", "Push Me!"))
					.OnClicked_Lambda([this]()
					{
						OnClicked2();
						OnClicked();
						return FReply::Handled();
					})
				]
			];

		FSlateApplication::Get().AddWindowAsNativeChild(window, GetGameViewportClient()->GetWindow().ToSharedRef(), true);
	}
	//));
}

void UMyGameInstance::InitSlateBrowser()
{
	//GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]()
	{
#if 1
		TSharedRef<SWebBrowser> browser = SNew(SWebBrowser)
			.InitialURL(TEXT("https://carvisualizer.plus360degrees.com/threejs/"))
			.ShowControls(false);
		m_browser = browser;
#endif
		TSharedRef<SWindow> window = SNew(SWindow)
			.Title(LOCTEXT("CreateWindow", "Test Slate Browser Window"))
			.ClientSize(FVector2D(800, 450.0f))
			[
#if 1
				browser
#else
				SNew(SWebBrowser)
				.InitialURL(TEXT("https://carvisualizer.plus360degrees.com/threejs/"))
				.ShowControls(false)
#endif
			];
		FSlateApplication::Get().AddWindowAsNativeChild(window, GetGameViewportClient()->GetWindow().ToSharedRef(), true);

		//JavaScriptからアクセス可能な変数を登録
		MyObject = NewObject<UMyObject>(UMyObject::StaticClass());
		MyObject->str = "initial";
//		m_browser->BindUObject("MyObject", MyObject, true);

		//JavaScriptから変数を更新
		const FString script = "window.ue4.MyObject.str = \"String is set!!\";";
		//m_browser->ExecuteJavascript(script);

	}
	//));
}

void UMyGameInstance::OnClicked2()
{
	//JavaScriptから変数を更新(うまく動いていない)
	m_browser->BindUObject("MyObject", MyObject, true);

	//FString script= "window.ue4.MyObject.str = \"String is set2!!\";";
	//FString script = "window.location.href=\"https://finance.yahoo.co.jp/\";";
	//FString script = "alert(\"AAAA\");";
	FString script =
"var toString = Object.prototype.toString;\
	alert(toString.call(window.ue4));\
";
	m_browser->ExecuteJavascript(script);//ue4変数が無いと言われる
}


#undef LOCTEXT_NAMESPACE 