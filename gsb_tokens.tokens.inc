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

  if ($type == 'node' && !empty($data['node'])) {
    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'menu-link:parents:parent-full-path':
          $menu = $data['node']->menu;
          if (!empty($menu['plid'])) {
            $menu_item = menu_link_load($menu['plid']);
            $replacements[$original] = drupal_get_path_alias($menu_item['link_path']);
          }
          else {
            if (isset($data['node']->menu['menu_name'])) {
              $menu_name = $data['node']->menu['menu_name'];
            }
            // If the data doesn't have the menu_name pull it straight from the
            // database.
            else {
              $menu_name = db_query("SELECT ml.menu_name from {menu_links} ml where link_path = 'node/" . $data['node']->nid . "'")->fetchField();
            }
            $tid = str_replace('section-', '', $menu_name);
            $section = taxonomy_term_load($tid);
            $replacements[$original] = $section->field_workbench_section_path[$data['node']->language][0]['safe_value'];
          }

          break;
      }
    }
  }

  return $replacements;
}
