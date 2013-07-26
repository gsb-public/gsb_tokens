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
          $node = $data['node'];
          // If we don't get the menu then pull it from the database.
          if (empty($node->menu)) {
            $menu = db_query("SELECT ml.menu_name, ml.plid from {menu_links} ml where link_path = 'node/" . $node->nid . "'")->fetchAssoc();
          }
          else {
            $menu = $node->menu;
          }

          $replacement = '';

          // If there is no menu then use content as the replacement.
          if (empty($menu)) {
            $replacement = 'content';
          }
          // If there is a parent then use the parents path as the replacement.
          elseif (!empty($menu['plid'])) {
            $menu_item = menu_link_load($menu['plid']);
            $replacement = drupal_get_path_alias($menu_item['link_path']);
          }
          // Otherwise get the path of the menu.
          else {
            // If data doesn't have the menu_name pull it from the database.
            if (!isset($menu['menu_name'])) { {
              $menu['menu_name'] = db_query("SELECT ml.menu_name from {menu_links} ml where link_path = 'node/" . $node->nid . "'")->fetchField();
            }

            // Make sure the menu is a section.
            if (stristr($menu['menu_name'], 'section-')) {
              $tid = str_replace('section-', '', $menu['menu_name']);
              $section = taxonomy_term_load($tid);
              $replacement = $section->field_workbench_section_path[$node->language][0]['safe_value'];
            }
          }

          $replacements[$original] = $replacement;
          break;
      }
    }
  }

  return $replacements;
}
