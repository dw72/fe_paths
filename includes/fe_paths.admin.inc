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
  $entities_array = array();

  foreach ($file_entity_info['bundles'] as $name => $bundle) {
    $file_types[$name] = $bundle['label'];
  }

  foreach ($entities as $name => $entity) {
    // Only works with fieldable entities.
    if ($entity['fieldable']) {
      $entities_array[$name] = $entity['label'];
    }
  }

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

  $form['file_entity'] = array(
    '#type' => 'fieldset',
    '#title' => t('File types'),
    'file_bundle' => array (
      '#type' => 'checkboxes',
      '#options' => $file_types,
      '#description' => t('Choose, in which file types should this configuration be activated.'),
    )
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
      )
    ),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
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
      '#title' => t('Bundle entity'),
      '#description' => t('Choose the parent entity. The parent entity determine, which type of tokens will be used in this config'),
      '#options' => array('global' => t('Global')) + $bundles,
      '#prefix' => '<div id="bundle-target">',
      '#suffix' => '</div><div id="fields-target"></div>',
      '#ajax' => array(
        //'callback' => 'fe_paths_add_bundle_settings',
        //'wrapper' => 'bundle-target',
      ),
    );
    return $form['bundle'];
  }
  else {
    return '<div id="bundle-target"></div>';
  }
}
