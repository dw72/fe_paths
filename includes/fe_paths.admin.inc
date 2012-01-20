<?php
/**
 * @file
 * Admin ui for the File Entity Paths module.
 */

function fe_paths_admin_form($form, &$form_state, $file_type_argument) {
  $settings = fe_paths_get_settings();

  $token_tree = array(
    '#type' => 'fieldset',
    '#title' => t('Replacement patterns'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => theme('token_tree', array('token_types' => array('file', 'file'))),
    '#weight' => 10,
  );
  $form = array(
    'public_settings' => array(
      '#type' => 'fieldset',
      '#title' => t('Public file'),
      '#description' => t('Default settings for public files.'),
      "fep_{$file_type_argument}_public_path" => array(
        '#type' => 'textfield',
        '#title' => t("Path for $file_type_argument"),
        '#element_validate' => array('token_element_validate'),
        '#token_types' => array('file', 'file'),
        '#field_prefix' => variable_get('file_public_path', conf_path() . '/files') . '/',
        '#default_value' => $settings[$file_type_argument]['public_path'],
      ),
      "fep_{$file_type_argument}_public_filename" => array(
        '#type' => 'textfield',
        '#title' => t("Filename for $file_type_argument"),
        '#element_validate' => array('token_element_validate'),
        '#token_types' => array('file', 'file'),
        '#default_value' => $settings[$file_type_argument]['public_filename'],
      ) + $token_tree,
    ),
    'private_settings' => array(
      '#type' => 'fieldset',
      '#title' => t('Private file'),
      '#description' => t('Default settings for private files.'),
      "fep_{$file_type_argument}_private_path" => array(
        '#type' => 'textfield',
        '#title' => t("Path for $file_type_argument"),
        '#element_validate' => array('token_element_validate'),
        '#token_types' => array('file', 'file'),
        '#field_prefix' => variable_get('file_private_path', conf_path() . '/files') . '/',
        '#default_value' => $settings[$file_type_argument]['private_path'],
      ),
      "fep_{$file_type_argument}_private_filename" => array(
        '#type' => 'textfield',
        '#title' => t("Filename for $file_type_argument"),
        '#element_validate' => array('token_element_validate'),
        '#token_types' => array('file', 'file'),
        '#default_value' => $settings[$file_type_argument]['private_filename'],
      ) + $token_tree,
    ),
  );

  return system_settings_form($form);
}