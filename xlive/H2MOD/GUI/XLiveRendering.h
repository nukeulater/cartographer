#pragma once

namespace XLiveRendering
{
	void InitializeD3D9(IDirect3DDevice9Ex* pD3DD, D3DPRESENT_PARAMETERS* presentParameters);
	void D3D9ReleaseResources();
};
