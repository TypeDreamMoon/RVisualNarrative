#pragma once

#include "CoreMinimal.h"
#include "ClassCollector/RVNClassCollector.h"

class SRVNStateWidget;

class SRVNStateInternal_Decorator : public SCompoundWidget
{
	friend class FRVNDecoratorDragDropOp;

	DECLARE_DELEGATE_OneParam(FOnDeleteDecoratorRequested, const FRVNClassInfo&)

public:
	SLATE_BEGIN_ARGS(SRVNStateInternal_Decorator)
		{
		}

		SLATE_ARGUMENT(FRVNClassInfo, DecoratorInfo)
		SLATE_ARGUMENT(TWeakPtr<SRVNStateWidget>, OuterNode)
		SLATE_EVENT(FOnDeleteDecoratorRequested, OnDeleteRequested)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	FORCEINLINE FRVNClassInfo GetDecoratorInfo() const { return DecoratorInfo; }

	FORCEINLINE TWeakPtr<SRVNStateWidget> GetOuterNode() const { return OuterNode.Pin(); }

protected:
	//~ Begin SWidget Interface
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override;
	virtual bool SupportsKeyboardFocus() const override { return true; }
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;
	//~ End SWidget Interface

private:
	void RequestDelete() const;

	void SetIsSelected(bool bSelected);
	bool IsSelected() const { return bIsSelected; }

public:
	FOnDeleteDecoratorRequested OnDeleteRequested;

private:
	FRVNClassInfo DecoratorInfo;

	TWeakPtr<SRVNStateWidget> OuterNode;

	TSharedPtr<SBorder> MainBorder;

	bool bIsSelected = false;

	static const FLinearColor ColorNormal;
	static const FLinearColor ColorHovered;
	static const FLinearColor ColorSelected;
	static const FLinearColor ColorDragging;
};
