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

  $info['tokens']['current-user']['faculty-profile-url'] = array(
    'name' => t('Faculty profile url'),
    'description' => t('Get the url to the current users faculty profile'),
  );

  $info['tokens']['current-user']['faculty-profile-edit-url'] = array(
    'name' => t('Faculty profile edit url'),
    'description' => t('Get the faculty profile edit url of the current user'),
  );

  $info['tokens']['node']['gsb-event-location'] = array(
    'name' => t('Event Location With View Mode'),
    'description' => t('Event Location with optional view mode. ex. gsb-event-location:[view-mode]'),
  );

  return $info;
}

/**
 * Implements hook_tokens().
 */
function gsb_tokens_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();

  if ($type == 'node' && !empty($data['node'])) {
    $node = $data['node'];
    foreach ($tokens as $name => $original) {
      $parameters = explode(':', $name);
      $name = array_shift($parameters);
      switch ($name) {
        case 'gsb-event-location':
          $replacement = '';
          foreach ($node->field_event_location[$node->language] as $location) {
            $pane_id = $location['value'];
            $field_event_location = entity_load('field_collection_item', array($pane_id));
            $view_mode = empty($parameters['0']) ? 'default' : $parameters[0];
            $entity_array = entity_view('field_collection_item', $field_event_location, $view_mode);
            $replacement .= drupal_render($entity_array);
          }
          $replacements[$original] = $replacement;
          break;
        case 'menu-link:parents:parent-full-path':
          $node = $data['node'];
          // If we don't get the menu then pull it from the database.
          if (empty($node->menu)) {
            $menu = db_query("SELECT ml.menu_name, ml.plid, ml.mlid from {menu_links} ml where link_path = 'node/" . $node->nid . "' AND menu_name <> 'navigation'")->fetchAssoc();
          }
          else {
            $menu = $node->menu;
          }

          $replacement = '';

          // If there is no menu then use content as the replacement.
          if (empty($menu['mlid'])) {
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
            if (!isset($menu['menu_name'])) {
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

  if ($type == 'current-user') {
    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'faculty-profile-url':
        case 'faculty-profile-edit-url':
          $nid = '';
          // Query for the faculty node this user belongs to.
          if (field_info_instance('node', 'field_sunetid', 'faculty')) {
            $query = new EntityFieldQuery('node');
            $results = $query
              ->entityCondition('bundle', 'faculty')
              ->fieldCondition('field_sunetid', 'value', $GLOBALS['user']->name)
              ->execute();
            if (!empty($results['node'])) {
              $nid = key($results['node']);
            }
          }

          if (is_numeric($nid)) {
            $path = 'node/' . $nid;
            if ($name == 'faculty-profile-edit-url') {
              $path .= '/edit';
            }

            $alias = drupal_get_path_alias($path);
            $replacements[$original] = $alias;
          }
          break;
      }
    }
  }

  return $replacements;
}
