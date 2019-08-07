/*
 * Modern effects for a modern Streamer
 * Copyright (C) 2017 Michael Fabian Dirks
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include "source-shader.hpp"
#include "strings.hpp"
#include "utility.hpp"

#define ST "Source.Shader"
#define ST_WIDTH ST ".Width"
#define ST_HEIGHT ST ".Height"

P_INITIALIZER(SourceShaderInit)
{
	initializer_functions.push_back([] { source::shader::shader_factory::initialize(); });
	finalizer_functions.push_back([] { source::shader::shader_factory::finalize(); });
}

static std::shared_ptr<source::shader::shader_factory> factory_instance = nullptr;

void source::shader::shader_factory::initialize()
{
	factory_instance = std::make_shared<source::shader::shader_factory>();
}

void source::shader::shader_factory::finalize()
{
	factory_instance.reset();
}

std::shared_ptr<source::shader::shader_factory> source::shader::shader_factory::get()
{
	return factory_instance;
}

static void get_defaults(obs_data_t* data)
{
	obs_data_set_default_int(data, ST_WIDTH, 1920);
	obs_data_set_default_int(data, ST_HEIGHT, 1080);
	obs_data_set_default_string(data, S_SHADER_FILE, obs_module_file("shaders/source/example.effect"));
	obs_data_set_default_string(data, S_SHADER_TECHNIQUE, "Draw");
}

source::shader::shader_factory::shader_factory()
{
	memset(&_source_info, 0, sizeof(obs_source_info));
	_source_info.id           = "obs-stream-effects-source-shader";
	_source_info.type         = OBS_SOURCE_TYPE_INPUT;
	_source_info.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW;
	_source_info.get_name     = [](void*) { return D_TRANSLATE(ST); };
	_source_info.get_defaults = get_defaults;

	_source_info.create = [](obs_data_t* data, obs_source_t* self) {
		try {
			return static_cast<void*>(new source::shader::shader_instance(data, self));
		} catch (std::exception& ex) {
			P_LOG_ERROR("<source-shader> Failed to create source, error: %s", ex.what());
		} catch (...) {
			P_LOG_ERROR("<source-shader> Failed to create source.");
		}
		return static_cast<void*>(nullptr);
	};
	_source_info.destroy = [](void* ptr) {
		try {
			delete reinterpret_cast<source::shader::shader_instance*>(ptr);
		} catch (std::exception& ex) {
			P_LOG_ERROR("<source-shader> Failed to delete source, error: %s", ex.what());
		} catch (...) {
			P_LOG_ERROR("<source-shader> Failed to delete source.");
		}
	};
	_source_info.get_properties = [](void* ptr) {
		obs_properties_t* pr = obs_properties_create();
		try {
			if (ptr)
				reinterpret_cast<source::shader::shader_instance*>(ptr)->properties(pr);
		} catch (std::exception& ex) {
			P_LOG_ERROR("<source-shader> Failed to retrieve options, error: %s", ex.what());
		} catch (...) {
			P_LOG_ERROR("<source-shader> Failed to retrieve options.");
		}
		return pr;
	};
	_source_info.get_width = [](void* ptr) {
		try {
			if (ptr)
				return reinterpret_cast<source::shader::shader_instance*>(ptr)->width();
		} catch (std::exception& ex) {
			P_LOG_ERROR("<source-shader> Failed to retrieve width, error: %s", ex.what());
		} catch (...) {
			P_LOG_ERROR("<source-shader> Failed to retrieve width.");
		}
		return uint32_t(0);
	};
	_source_info.get_height = [](void* ptr) {
		try {
			if (ptr)
				return reinterpret_cast<source::shader::shader_instance*>(ptr)->height();
		} catch (std::exception& ex) {
			P_LOG_ERROR("<source-shader> Failed to retrieve height, error: %s", ex.what());
		} catch (...) {
			P_LOG_ERROR("<source-shader> Failed to retrieve height.");
		}
		return uint32_t(0);
	};
	_source_info.update = [](void* ptr, obs_data_t* data) {
		try {
			if (ptr)
				reinterpret_cast<source::shader::shader_instance*>(ptr)->update(data);
		} catch (std::exception& ex) {
			P_LOG_ERROR("<source-shader> Failed to update, error: %s", ex.what());
		} catch (...) {
			P_LOG_ERROR("<source-shader> Failed to update.");
		}
	};
	_source_info.activate = [](void* ptr) {
		try {
			if (ptr)
				reinterpret_cast<source::shader::shader_instance*>(ptr)->activate();
		} catch (std::exception& ex) {
			P_LOG_ERROR("<source-shader> Failed to activate, error: %s", ex.what());
		} catch (...) {
			P_LOG_ERROR("<source-shader> Failed to activate.");
		}
	};
	_source_info.deactivate = [](void* ptr) {
		try {
			if (ptr)
				reinterpret_cast<source::shader::shader_instance*>(ptr)->deactivate();
		} catch (std::exception& ex) {
			P_LOG_ERROR("<source-shader> Failed to deactivate, error: %s", ex.what());
		} catch (...) {
			P_LOG_ERROR("<source-shader> Failed to deactivate.");
		}
	};
	_source_info.video_tick = [](void* ptr, float_t time) {
		try {
			if (ptr)
				reinterpret_cast<source::shader::shader_instance*>(ptr)->video_tick(time);
		} catch (std::exception& ex) {
			P_LOG_ERROR("<source-shader> Failed to tick, error: %s", ex.what());
		} catch (...) {
			P_LOG_ERROR("<source-shader> Failed to tick.");
		}
	};
	_source_info.video_render = [](void* ptr, gs_effect_t* effect) {
		try {
			if (ptr)
				reinterpret_cast<source::shader::shader_instance*>(ptr)->video_render(effect);
		} catch (std::exception& ex) {
			P_LOG_ERROR("<source-shader> Failed to render, error: %s", ex.what());
		} catch (...) {
			P_LOG_ERROR("<source-shader> Failed to render.");
		}
	};

	obs_register_source(&_source_info);
}

source::shader::shader_factory::~shader_factory() {}

source::shader::shader_instance::shader_instance(obs_data_t* data, obs_source_t* self)
	: _self(self), _active(true), _width(0), _height(0)
{
	_fx = std::make_shared<gfx::effect_source::effect_source>();
	_fx->set_valid_property_cb(std::bind(&source::shader::shader_instance::valid_param, this, std::placeholders::_1));
	_fx->set_override_cb(std::bind(&source::shader::shader_instance::override_param, this, std::placeholders::_1));

	update(data);
}

source::shader::shader_instance::~shader_instance() {}

uint32_t source::shader::shader_instance::width()
{
	return _width;
}

uint32_t source::shader::shader_instance::height()
{
	return _height;
}

void source::shader::shader_instance::properties(obs_properties_t* props)
{
	obs_properties_add_int(props, ST_WIDTH, D_TRANSLATE(ST_WIDTH), 1, 32768, 1);
	obs_properties_add_int(props, ST_HEIGHT, D_TRANSLATE(ST_HEIGHT), 1, 32768, 1);

	_fx->properties(props);
}

void source::shader::shader_instance::update(obs_data_t* data)
{
	_width = obs_data_get_int(data, ST_WIDTH);
	_height = obs_data_get_int(data, ST_HEIGHT);

	_fx->update(data);
}

void source::shader::shader_instance::activate()
{
	_active = true;
}

void source::shader::shader_instance::deactivate()
{
	_active = false;
}

bool source::shader::shader_instance::valid_param(std::shared_ptr<gs::effect_parameter> param)
{
	return true;
}

void source::shader::shader_instance::override_param(std::shared_ptr<gs::effect> effect) {}

void source::shader::shader_instance::video_tick(float_t sec_since_last)
{
	_fx->tick(sec_since_last);
}

void source::shader::shader_instance::video_render(gs_effect_t*)
{
	// Grab initial values.
	gs_effect_t*  effect_default = obs_get_base_effect(obs_base_effect::OBS_EFFECT_DEFAULT);

	// Skip filter if anything is wrong.
	if (!_active || !_width || !_height || !effect_default) {
		obs_source_skip_video_filter(_self);
		return;
	}

	try {
		_fx->render();
	} catch (...) {
		obs_source_skip_video_filter(_self);
	}
}
