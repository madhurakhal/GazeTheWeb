//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "Renderer.h"
#include "src/CEF/Mediator.h"
#include "src/Utils/Texture.h"
#include "src/Utils/Logger.h"
#include "include/wrapper/cef_helpers.h"

Renderer::Renderer(Mediator* pMediator)
{
    _mediator = pMediator;
}

bool Renderer::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
{
    int width = 0, height = 0;
    _mediator->GetResolution(browser, width, height);
    rect = CefRect(0, 0, width, height);
    if (width == 0 || height == 0)
    {
        LogDebug("Renderer: GetViewRect size equal zero!");
    }
    return true;
}

void Renderer::OnPaint(
    CefRefPtr<CefBrowser> browser,
    PaintElementType type,
    const RectList &dirtyRects,
    const void *buffer,
    int width,
    int height)
{
    // Look up corresponding texture
    if (auto spTexture = _mediator->GetTexture(browser).lock())
    {
        // Fill texture with rendered website
        spTexture->Fill(width, height, GL_BGRA, (const unsigned char*) buffer);


		// TESTING
		//if (dirtyRects.size() > 0)
		//	LogDebug("*** Dirty rects: ***");

		//for (const auto& rect : dirtyRects)
		//{
		//	LogDebug(rect.width, " x ", rect.height, " - ", rect.x, ", ", rect.y);

		//	spTexture->drawRectangle(rect.width, rect.height, rect.x, rect.y);
		//}

    }
    else
    {
        LogDebug("Renderer: OnPaint couldn't fill texture...");
    }
}

void Renderer::OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser, double x, double y)
{
    // Call Mediator to set offset in corresponding Tab
    _mediator->OnScrollOffsetChanged(browser, x, y);
}
