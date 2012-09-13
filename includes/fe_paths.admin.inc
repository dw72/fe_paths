<?php
/**
 * @file
 * Admin ui for the File Entity Paths module.
 */

function fe_paths_admin_form($form, &$form_state) {
  $allowed = array(
    'public',
    'private'
  );
  $settings = fe_paths_get_settings();
  $entity_info = entity_get_info('file');
  foreach ($entity_info['bundles'] as $file_type => $bundle_info) {
    $form["fep_$file_type"] = array(
      '#type' => 'fieldset',
      '#title' => t("Global settings for $file_type"),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#tree' => TRUE,
    );
    foreach (file_get_stream_wrappers() as $scheme => $wrapper) {
      if (in_array($scheme, $allowed)) {
        $form["fep_$file_type"][$scheme] = array(
          '#type' => 'fieldset',
          '#title' => t("@scheme files", array('@scheme' => $scheme)),
          '#description' => t('Default settings for @scheme files.', array('@scheme' => $scheme)),
          '#tree' => TRUE,
          'path' => array(
            '#type' => 'textfield',
            '#title' => t("Path"),
            '#element_validate' => array('token_element_validate'),
            '#token_types' => array('file', 'file'),
            // Tmp disable until figure out, how to get the proper directory.
            //'#field_prefix' => variable_get('file_public_path', conf_path() . '/files') . '/',
            '#field_prefix' => "$scheme://",
            '#default_value' => $settings[$file_type][$scheme]['path'],
          ),
          'filename' => array(
            '#type' => 'textfield',
            '#title' => t("Filename"),
            '#element_validate' => array('token_element_validate'),
            '#token_types' => array('file', 'file'),
            '#default_value' => $settings[$file_type][$scheme]['filename'],
          )
        );
      }
    }
    $form["fep_$file_type"]['tokens'] = array(
      '#type' => 'fieldset',
      '#title' => t('Replacement patterns'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => theme('token_tree', array(
        'token_types' => array(
          'file',
          'file'
        )
      )),
      '#weight' => 1000,
    );
  }

  return system_settings_form($form);
}

function fe_paths_entity_edit_form($form, &$form_state, $config = NULL) {
  $file_entity_info = entity_get_info('file');
  $entities = entity_get_info();
  $file_types = array();
  $file_types_defaults = array();
  $entities_array = array();
  $token_info = token_get_info();

  if (!isset($form_state['fe_path_config'])) {
    $form_state['fe_path_config'] = $config;
  }

  foreach ($file_entity_info['bundles'] as $name => $bundle) {
    $file_types[$name] = $bundle['label'];
  }

  foreach ($entities as $name => $entity) {
    // Only works with fieldable entities.
    if ($entity['fieldable']) {
      $entities_array[$name] = $entity['label'];
    }
  }

  if (!is_null($config)) {
    $form['id'] = array(
      '#type' => 'hidden',
      '#default_value' => $config->id,
    );
  }

  $form['data'] = array(
    '#type' => 'container',
  );

  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#default_value' => isset($config->label) ? $config->label : '',
    '#required' => TRUE,
  );
  $form['machine_name'] = array(
    '#type' => 'machine_name',
    '#title' => t('Machine name'),
    '#default_value' => isset($config->machine_name) ? $config->machine_name : '',
    '#machine_name' => array(
      'exists' => 'fe_paths_check_machine_name',
    ),
  );

  $form['path_name'] = array(
    '#type' => 'fieldset',
    '#title' => t('Path and filename settings'),
    'path' => array(
      '#type' => 'textfield',
      '#title' => t("Path"),
      '#description' => t('The path applied after the sheme://. It means, if the file has scheme "public", and this field is set as "foo", the file will be moved to the public://foo path.'),
      //'#element_validate' => array('token_element_validate'),
      //'#token_types' => array('file', 'file'),
      // Tmp disable until figure out, how to get the proper directory.
      //'#field_prefix' => variable_get('file_public_path', conf_path() . '/files') . '/',
      '#field_prefix' => "sheme://",
      '#default_value' => isset($config->path) ? $config->path : '',
    ),
    'filename' => array(
      '#type' => 'textfield',
      '#title' => t("Filename"),
      '#description' => t('Set the filename pattern. This pattern will be validated based on parent entity settings. If settings is "global", all types of tokens are enabled. File tokens is always accepted.'),
      //'#element_validate' => array('token_element_validate'),
      //'#token_types' => array('file', 'file'),
      '#default_value' => isset($config->filename) ? $config->filename : '[file:name-only-original].[file:extension-original]',
    ),
    'tokens' => array(
      '#type' => 'fieldset',
      '#title' => t('Replacement patterns'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => theme('token_tree', array('token_types' => array_keys($entities_array))),
      '#weight' => 1000,
      '#prefix' => '<div id="fe-paths-tokens">',
      '#suffix' => '</div>',
    ),
  );

  // Fill out the default values of the file_bundle checkboxes.
  foreach ($config->data['file_entity'] as $key => $val) {
    if ($key === $val) {
      $file_types_defaults[] = $key;
    }
  }

  $form['file_entity'] = array(
    '#type' => 'fieldset',
    '#title' => t('File types'),
    'file_bundle' => array (
      '#type' => 'checkboxes',
      '#options' => $file_types,
      '#description' => t('Choose, in which file types should this configuration be activated.'),
      '#parents' => array('data', 'file_entity'),
      '#default_value' => $file_types_defaults,
    ),
  );


  $form['parent_entity'] = array(
    '#type' => 'fieldset',
    '#title' => t('Parent entity settings'),
    '#tree' => TRUE,
    'entity' => array(
      '#type' => 'select',
      '#title' => t('Choose entity'),
      '#description' => t('Choose the parent entity. The parent entity determine, which type of tokens will be used in this config'),
      '#options' => array('global' =>t('Global')) + $entities_array,
      '#suffix' => '<div id="bundle-target"></div>',
      '#ajax' => array(
        'callback' => 'fe_paths_add_bundle_settings',
        'wrapper' => 'bundle-target',
      ),
      '#parents' => array('data', 'entity'),
    ),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  dsm(get_defined_vars());
  return $form;
}

function fe_paths_add_bundle_settings($form, $form_state) {
  if (isset($form_state['values']['parent_entity']['entity']) && $form_state['values']['parent_entity']['entity'] != 'global') {
    $bundles = array();
    $name = $form_state['values']['parent_entity']['entity'];
    $entity = entity_get_info($name);
    foreach ($entity['bundles'] as $key => $bundle) {
      $bundles[$key] = $bundle['label'];
    }

    $form['bundle'] =  array(
      '#type' => 'select',
      '#title' => t('Bundle'),
      '#description' => t('Choose the parent entity. The parent entity determine, which type of tokens will be used in this config'),
      '#options' => array('global' => t('Global')) + $bundles,
      '#prefix' => '<div id="bundle-target">',
      '#suffix' => '</div><div id="fields-target"></div>',
      '#ajax' => array(
        //'callback' => 'fe_paths_add_bundle_settings',
        //'wrapper' => 'bundle-target',
      ),
      '#parents' => array('data'),
    );
    return $form['bundle'];
  }
  else {
    return '<div id="bundle-target"></div>';
  }
}

function fe_paths_entity_edit_form_validate($form, &$form_state) {
  $form_state['redirect'] = 'admin/config/media/fe-paths';
}

function fe_paths_entity_edit_form_submit($form, &$form_state) {
  $config = $form_state['values'];
  fe_paths_prepare_config($config);
  fe_paths_config_save($config);
}

function fe_paths_global_page() {
  $configs = fe_paths_config_load_multiple();
  $build = array();

  $build['config_form'] = drupal_get_form('fe_paths_config_table_form', $configs);
  $build['global_form'] = drupal_get_form('fe_paths_admin_form');
  return $build;
}

function fe_paths_config_table_form($form, &$form_state, $configs) {
  $form_state['configs'] = $configs;
  $form = array(
    'config' => array(
      '#prefix' => '<div id="config-order-form">',
      '#suffix' => '</div>',
      '#tree' => TRUE,
      '#theme' => 'fe_paths_config_table'
    )
  );
  foreach ($configs as $name => $config) {
    $form['config'][$name]['label'] = array(
      '#markup' => check_plain($config->label),
    );
    $form['config'][$name]['weight'] = array(
      '#type' => 'textfield',
      '#default_value' => $config->weight,
      '#size' => 3,
      '#attributes' => array('class' => array('order-weight')),
    );
    $form['config'][$name]['machine_name'] = array(
      '#markup' => check_plain($config->machine_name),
    );
    $form['config'][$name]['status'] = array(
      '#markup' => $config->status ? t('Enabled') : t('Disabled'),
    );
    $form['config'][$name]['path'] = array(
      '#markup' => check_plain($config->machine_name),
    );
    $form['config'][$name]['filename'] = array(
      '#markup' => check_plain($config->filename),
    );
    $form['config'][$name]['data'] = array(
      '#markup' => 'some data callback',
    );
    $form['config'][$name]['link'] = array(
      '#markup' => l(t('Edit'), "admin/config/media/fe-paths/{$config->id}/edit")
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Order'),
  );

  return $form;
}

function fe_paths_config_table_form_submit($form, &$form_state) {
  foreach ($form_state['configs'] as $id => $config) {
    $config->weight = $form_state['values']['config'][$id]['weight'];
    fe_paths_config_save($config);
  }
}

function theme_fe_paths_config_table($variables) {
  $element = $variables['element'];
  drupal_add_tabledrag('form_id', 'order', 'sibling', 'order-weight');
  $header = array(
    'label' => t('Label'),
    'weight' => t('Weight'),
    'machine_name' => t('Machine name'),
    'status' => t('Status'),
    'path' => t('Path'),
    'filename' => t('Filename'),
    'data' => t('Settings'),
    'link' => t('Edit'),
  );

  $rows = array();
  foreach (element_children($element) as $key) {
    $row = array();

    $row['data'] = array();
    foreach ($header as $fieldname => $title) {
      $row['data'][] = drupal_render($element[$key][$fieldname]);
      $row['class'] = array('draggable'); // needed for table dragging
    }
    $rows[] = $row;
  }


  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'form_id'), // needed for table dragging
  ));
}

function fe_paths_config_edit_page($config) {
  $build = drupal_get_form('fe_paths_entity_edit_form', $config);

  return $build;
}
