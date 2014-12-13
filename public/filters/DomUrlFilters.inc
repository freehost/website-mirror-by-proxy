<?php

class DomUrlFilters
{

	private static $attributes = array();

	public static function addAttribute($attribute, $element = '*')
	{
		self::$attributes[$attribute] = $element;
	}

	public static function applyAll(simple_html_dom $dom)
	{
		foreach (self::$attributes as $attribute => $elements) {
			self::apply($dom, $attribute, $elements);
		}
	}

	private static function apply(simple_html_dom $dom, $attribute, $elements)
	{
		foreach ($dom->find($elements . '[' . $attribute . ']') as $element) {
			$attr_val = $element->getAttribute($attribute);
			if ($attr_val) {
				
				// Turn protocol relative URLs into HTTPS.
				if($attr_val[1] == '/' && $attr_val[0] == '/') {
					$attr_val = 'https:' . $attr_val;
					$element->setAttribute($attribute, $attr_val);
					continue;
				}
				
				$attr_val_components = parse_url($attr_val);
				
				// Remove default host.
				if (isset($attr_val_components['host']) && $attr_val_components['host'] ==
					 Conf::getDefaultUpstreamBaseUrlComponent('host')) {
					unset($attr_val_components['host']);
				}
				
				// If relative URL.
				if (! isset($attr_val_components['host'])) {
					$attr_val = '.' .
						 http_build_path_query_fragment($attr_val_components);
					$element->setAttribute($attribute, $attr_val);
				}
				
				/*
				 * Special for existing base href values. They should always end with a slash, 
				 * but browsers are lenient if it's eg http://example.com. But since we may rewrite
				 * the value later, it might have a path and without the slash it will fail.
				 */
				if ($element->tag == 'base') {
					$last_char = $attr_val[strlen($attr_val) - 1];
					if ($last_char != '/') {
						$attr_val .= '/';
						$element->setAttribute($attribute, $attr_val);
					}
				}
			}
		}
	}
}