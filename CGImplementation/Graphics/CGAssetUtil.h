#pragma once
#ifndef __CG_ASSET_UTIL_H__
#define __CG_ASSET_UTIL_H__

namespace CGProj
{
	enum CG_SHADER_ENUM
	{
		SHADER_DEFERRED_FIRST,
		SHADER_DEFERRED_SECOND,
		SHADER_CG_LINE,
		SHADER_SIMPLE_RENDER,
		SHADER_SIMPLE_COLOR_RENDER,
		SHADER_WIRE_RENDER,
		NUM_CG_SHADER_ENUM
	};

	enum CG_TEXTURE_ENUM
	{
		TEXTURE_CONTAINER_DIFFUSE,
		TEXTURE_CONTAINER_SPECULAR,
		TEXTURE_FIELD_GRASS,
		TEXTURE_GOLD,
		TEXTURE_MARBLE,
		TEXTURE_RED_MARBLE,
		TEXTURE_MATRIX,
		TEXTURE_WOOD_PANEL,
		NUM_CG_TEXTURE_ENUM
	};
}

#endif 