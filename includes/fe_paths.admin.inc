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

function fe_paths_entity_edit_form($form, &$form_state) {
  $file_entity_info = entity_get_info('file');
  $form['file_entity'] = array(
    '#type' => 'fieldset',
    '#title' => t('File bundle'),
    'file_bundle' => array (
      '#type' => 'checkboxes',
      '#options' => $file_entity_info['bundles'],
      '#title' => t('File bundle'),
      '#description' => t('Choose, in which file types should this settings be activated.'),
    )
  );
}
