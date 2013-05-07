<?php

/**
 * @file
 * Token integration for the Pathauto module.
 */

/**
 * Implements hook_token_info().
 */
function gsb_tokens_token_info() {
  $info = array();

  $info['tokens']['array']['parent-full-path'] = array(
    'name' => t('Get the parent full path'),
    'description' => t('Returns the parent menu items\' path alias.'),
  );

  return $info;
}

/**
 * Implements hook_tokens().
 */
function gsb_tokens_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();

  if ($type == 'array' && !empty($data['array'])) {
    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'parent-full-path';
          $parent_mlid = key(array_pop($data['array']));
          $menu_item = menu_link_load($parent_mlid);
          $replacements[$original] = drupal_get_path_alias($menu_item['link_path']);
          break;
      }
    }
  }

  return $replacements;
}
