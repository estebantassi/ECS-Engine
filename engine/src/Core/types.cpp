#include <core/types.h>

PointLightShader ConvertPointLight(PointLight light, EntityID id, Transform transform)
{
	return {
	transform.position,
	id,
	light.color,
	light.intensity,
	light.reflectionColor,
	light.radius,
	};
}

SpotLightShader ConvertSpotLight(SpotLight light, EntityID id, Transform transform)
{
	glm::vec3 direction = transform.rotation * glm::vec3(0, 0, 1);

	return {
		transform.position,
		id,
		light.color,
		light.intensity,
		light.reflectionColor,
		light.radius,
		direction,
		light.innerCutoff,
		light.outerCutoff
	};
}