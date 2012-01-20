<?php
/**
 * @file
 * Admin ui for the File Entity Paths module.
 */

function fe_paths_admin_form($form, &$form_state, $file_type_argument) {
  $settings = fe_paths_get_settings($file_type_argument);

  $token_tree = array(
    '#type' => 'fieldset',
    '#title' => t('Replacement patterns'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => theme('token_tree', array('token_types' => array('file', 'file'))),
    '#weight' => 10,
  );
  $form["fep_$file_type_argument"] = array(
    '#tree' => true,
    'default' => array(
      '#type' => 'fieldset',
      '#title' => t('Defaults'),
      '#description' => t('Default settings for files.'),
      'path' => array(
        '#type' => 'textfield',
        '#title' => t("Path for $file_type_argument"),
        '#element_validate' => array('token_element_validate'),
        '#token_types' => array('file', 'file'),
        '#field_prefix' => variable_get('file_public_path', conf_path() . '/files') . '/',
        '#default_value' => $settings['default']['path'],
      ),
      'filename' => array(
        '#type' => 'textfield',
        '#title' => t("Filename for $file_type_argument"),
        '#element_validate' => array('token_element_validate'),
        '#token_types' => array('file', 'file'),
        '#default_value' => $settings['default']['filename'],
      ) + $token_tree,
    ),
    'private' => array(
      '#type' => 'fieldset',
      '#title' => t('Private file'),
      '#description' => t('Default settings for private files.'),
      'path' => array(
        '#type' => 'textfield',
        '#title' => t("Path for $file_type_argument"),
        '#element_validate' => array('token_element_validate'),
        '#token_types' => array('file', 'file'),
        '#field_prefix' => variable_get('file_private_path', conf_path() . '/files') . '/',
        '#default_value' => $settings['private']['path'],
      ),
      'filename' => array(
        '#type' => 'textfield',
        '#title' => t("Filename for $file_type_argument"),
        '#element_validate' => array('token_element_validate'),
        '#token_types' => array('file', 'file'),
        '#default_value' => $settings['private']['filename'],
      ) + $token_tree,
    ),
  );

  return system_settings_form($form);
}