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

  $info['tokens']['array']['gsb-join-path'] = array(
    'name' => t('Custom token for GSB'),
    'description' => t('The array values each cleaned by Pathauto and then joined with the slash into a string that resembles an URL.'),
  );

  return $info;
}

/**
 * Implements hook_tokens().
 */
function gsb_tokens_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();

  if ($type == 'array' && !empty($data['array'])) {
    $array = $data['array'];
    
    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'gsb-join-path';
          end($array);
          $menu_item = menu_link_load(key($array));
          $replacements[$original] = drupal_get_path_alias($menu_item['link_path']);
          break;
      }
    }
  }

  return $replacements;
}
