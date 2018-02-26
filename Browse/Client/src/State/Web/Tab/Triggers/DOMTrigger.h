//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Abstract trigger generated from DOM elements. Template structure to be used
// as trigger from various DOM node types.

#ifndef DOMTRIGGER_H_
#define DOMTRIGGER_H_

#include "src/State/Web/Tab/Triggers/Trigger.h"
#include "src/CEF/Data/DOMNode.h"
#include "src/Global.h"
#include "src/Setup.h"
#include "submodules/eyeGUI/include/eyeGUI.h"
#include <memory>

template <class T>
class DOMTrigger : public Trigger
{
public:

    // Constructor
	DOMTrigger(TabInteractionInterface* pTab, std::vector<Trigger*>& rTriggerCollection, std::shared_ptr<T> spNode, std::string brickPath, std::string idExtension);

    // Destructor
    virtual ~DOMTrigger() = 0;

    // Update
    virtual bool Update(float tpf, const std::shared_ptr<const TabInput> spInput);

    // Draw
    virtual void Draw() const;

    // Activate
    virtual void Activate();

    // Deactivate
    virtual void Deactivate();

	// Trigger in next update
	virtual void Schedule() { _scheduled = true; }

	// Get rects of DOMNode
    std::vector<Rect> GetDOMRects() const { return _spNode->GetRects(); }

    // Get whether DOMNode is marked as fixed
    bool GetDOMFixed() const { return _spNode->GetFixedId() >= 0; } // TODO: call real "isFixed" method so not checked for being zero

protected:

	// Shared pointer to node
	std::shared_ptr<T> _spNode;

private:

	// Available offsets within DOM node in x direction
	const std::vector<float> _offsets = { 0.5f, 0.7f, 0.3f, 0.9f, 0.1f };

    // Update position of overlay
    void UpdatePositionOfOverlayFrame(bool isButton); // button or badge

	// Calculate width of badge overlay
	float CalculateWidthOfBadgeOverlay() const;

    // Index of floating frame in Tab's overlay for button
    int _overlayButtonFrameIndex = -1;

    // Id of button in overlay
    std::string _overlayButtonId;

	// Index of floating frame in Tab's overlay for badge
	int _overlayBadgeFrameIndex = -1;

	// Id of badge in overlay
	std::string _overlayBadgeId;

    // Bool to remember that it was triggered
    bool _triggered = false;

	// Scheduled triggering
	bool _scheduled = false;

    // Visibility of overlay
    bool _visible = false;

	// Store offset of button to move badge accordingly
	float _buttonOffset = 0.f;
};

// ######################
// ### IMPLEMENTATION ###
// ######################

template <class T>
DOMTrigger<T>::DOMTrigger(TabInteractionInterface* pTab, std::vector<Trigger*>& rTriggerCollection, std::shared_ptr<T> spNode, std::string brickPath, std::string idExtension) : Trigger(pTab, rTriggerCollection)
{
	// Save member
	_spNode = spNode;

	// ### BUTTON ###

	// Create id, which is unique in overlay
	_overlayButtonId = "dom_trigger_button" + idExtension + "_" + std::to_string(_spNode->GetId());

	// Id mapper for brick
	std::map<std::string, std::string> idMapper;
	idMapper.emplace("button", _overlayButtonId);

	// Add overlay
	_overlayButtonFrameIndex = _pTab->AddFloatingFrameToOverlay(brickPath, 0, 0, TAB_TRIGGER_BUTTON_SIZE, TAB_TRIGGER_BUTTON_SIZE, idMapper);

	// Register listener
	_pTab->RegisterButtonListenerInOverlay(
		_overlayButtonId,
		[&]() { this->_triggered = true; }, // it is checked for triggered in update
		[]() {}); // no down

	// ### BADGE ###

	if (setup::TAB_TRIGGER_SHOW_BADGE)
	{
		// Create id, which is unique in overlay
		_overlayBadgeId = "dom_trigger_badge" + idExtension + "_" + std::to_string(_spNode->GetId());

		// Id mapper for brick
		idMapper.clear();
		idMapper.emplace("text", _overlayBadgeId);

		// Add overlay
		_overlayBadgeFrameIndex = _pTab->AddFloatingFrameToOverlay("bricks/triggers/IdentifierBadge.beyegui", 0, 0, CalculateWidthOfBadgeOverlay(), TAB_TRIGGER_BADGE_SIZE, idMapper);

		// Set content of text
		std::u16string id16;
		eyegui_helper::convertUTF8ToUTF16(std::to_string(_spNode->GetId() + 1), id16); // id is displayed with value + 1 for usability
		_pTab->SetContentOfTextBlock(_overlayBadgeId, id16);
	}
}

template <class T>
DOMTrigger<T>::~DOMTrigger()
{
	// ### BUTTON ###

	// Delete overlay frame
	_pTab->RemoveFloatingFrameFromOverlay(_overlayButtonFrameIndex);

	// Unregister button from overlay
	_pTab->UnregisterButtonListenerInOverlay(_overlayButtonId);

	// ### BADGE ###

	if (setup::TAB_TRIGGER_SHOW_BADGE)
	{
		// Delete overlay frame
		_pTab->RemoveFloatingFrameFromOverlay(_overlayBadgeFrameIndex);
	}
}

template <class T>
bool DOMTrigger<T>::Update(float tpf, const std::shared_ptr<const TabInput> spInput)
{
	// Decide visibility
	bool visible =
		// !_spNode->IsOccluded() && // node is not occluded
		!_spNode->GetRects().empty() && // DOM node has rects
		_spNode->GetRects().front().Width() != 0 && _spNode->GetRects().front().Height() != 0; // At least the first rect is bigger than zero
	if (_visible != visible)
	{
		_visible = visible;
		_pTab->SetVisibilityOfFloatingFrameInOverlay(_overlayButtonFrameIndex, _visible); // button
		if (setup::TAB_TRIGGER_SHOW_BADGE)
		{
			_pTab->SetVisibilityOfFloatingFrameInOverlay(_overlayBadgeFrameIndex, _visible); // badge
		}
	}

	// ### BUTTON ###

	// Update position of overlay button
	UpdatePositionOfOverlayFrame(true);

	// ### BADGE ###

	if (setup::TAB_TRIGGER_SHOW_BADGE)
	{
		// Update position of overlay badge
		UpdatePositionOfOverlayFrame(false);

		// Update size
		_pTab->SetSizeOfFloatingFrameInOverlay(_overlayBadgeFrameIndex, CalculateWidthOfBadgeOverlay(), TAB_TRIGGER_BADGE_SIZE);
	}

	// #############

	// Remember about being triggered
	bool triggered = _triggered || _scheduled;
	_triggered = false;
	_scheduled = false; // also reset scheduled trigger

	// Return true whether triggered
	return triggered;
}

template <class T>
void DOMTrigger<T>::Draw() const
{
	// Nothing to draw here, since eyeGUI does it
}

template <class T>
void DOMTrigger<T>::Activate()
{
	_pTab->SetVisibilityOfFloatingFrameInOverlay(_overlayButtonFrameIndex, _visible); // button
	if (setup::TAB_TRIGGER_SHOW_BADGE)
	{
		_pTab->SetVisibilityOfFloatingFrameInOverlay(_overlayBadgeFrameIndex, _visible); // badge
	}
}

template <class T>
void DOMTrigger<T>::Deactivate()
{
	_pTab->SetVisibilityOfFloatingFrameInOverlay(_overlayButtonFrameIndex, false); // button
	if (setup::TAB_TRIGGER_SHOW_BADGE)
	{
		_pTab->SetVisibilityOfFloatingFrameInOverlay(_overlayBadgeFrameIndex, false); // badge
	}
}

template <class T>
void DOMTrigger<T>::UpdatePositionOfOverlayFrame(bool isButton)
{
	// Scrolling offset only when not fixed
	double scrollingOffsetX = 0;
	double scrollingOffsetY = 0;
	if (!GetDOMFixed())
	{
		_pTab->GetScrollingOffset(scrollingOffsetX, scrollingOffsetY);
	}

	if (_spNode->GetRects().size() > 0)
	{
		// Determine size
		float size = isButton ? TAB_TRIGGER_BUTTON_SIZE : TAB_TRIGGER_BADGE_SIZE;

		// Center of node
		const auto& nodeCenter = _spNode->GetRects()[0].Center();
		const auto nodeWidth = _spNode->GetRects()[0].Width();

		// Function to go from CEF pixels to relative screen position
		std::function<glm::vec2(const glm::vec2&)> ToRelativeScreenPosition = [&](const glm::vec2& rCEFPixel)
		{
			double webViewPixelX = rCEFPixel.x - scrollingOffsetX;
			double webViewPixelY = rCEFPixel.y - scrollingOffsetY;
			_pTab->ConvertToWebViewPixel(webViewPixelX, webViewPixelY);
			return glm::vec2(
				((float)webViewPixelX + (float)_pTab->GetWebViewX()) / (float)_pTab->GetWindowWidth(),
				((float)webViewPixelY + (float)_pTab->GetWebViewY()) / (float)_pTab->GetWindowHeight());
		};

		/*

		// If button, try to find empty space
		if (isButton)
		{
			// Go over triggers so far
			int buttonOffsetIndex = 0;
			for (int i = 0; i < (int)_pTriggerCollection->size(); i++)
			{
				// Check for different kind of triggers
				auto pTrigger = _pTriggerCollection.at(i);
				if (i = 0) // this if first trigger, just place it conveniently
				{
					buttonOffset = _offset.at(0);
				}
				else if(pTrigger == nullptr) // ups
				{
					continue;
				} // TODO: if trigger on same original position as this, use its placement
				else if (pTrigger == this) // ok this is this so stop it
				{
					break;
				}
				else
				{
					// Determine buttonOffset (may only become bigger)
					for (int j = buttonOffsetIndex; j < (int)_offsets; j++)
					{

						if()
					}
				}
			}
		}

		*/

		// Relative horizontal page pixel position of trigger within node
		auto xCoord = _spNode->GetRects()[0].left + (nodeWidth * 0.5f);

		// Calculate relative screen position
		glm::vec2 relativePosition = ToRelativeScreenPosition(glm::vec2(xCoord, nodeCenter.y));

		// Subtract half of size to center frame
		relativePosition.x -= size / 2.f;
		relativePosition.y -= size / 2.f;

		// If badge, add offset
		if (!isButton)
		{
			relativePosition.x += TAB_TRIGGER_BADGE_OFFSET.x;
			relativePosition.y += TAB_TRIGGER_BADGE_OFFSET.y;

			// Tell it floating frames
			_pTab->SetPositionOfFloatingFrameInOverlay(_overlayBadgeFrameIndex, relativePosition.x, relativePosition.y);
		}
		else
		{
			// Tell it floating frames
			_pTab->SetPositionOfFloatingFrameInOverlay(_overlayButtonFrameIndex, relativePosition.x, relativePosition.y);
		}
	}
}

template <class T>
float DOMTrigger<T>::CalculateWidthOfBadgeOverlay() const
{
	return TAB_TRIGGER_BADGE_SIZE * ((float)_pTab->GetWindowHeight() / (float)_pTab->GetWindowWidth());
}

#endif // DOMTRIGGER_H_
