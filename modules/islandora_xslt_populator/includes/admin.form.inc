<?php

/**
 * @file
 * Admin form and associated functions.
 */

/**
 * Form building functions; allow addition and removal of XSLT populators.
 */
function islandora_xslt_populator_admin_form($form, &$form_state) {
  form_load_include($form_state, 'inc', 'islandora_xslt_populator', 'includes/db');
  $populators = islandora_xslt_populator_get_populators();
  foreach ($populators as &$populator) {
    $populator['description'] = check_plain($populator['description']);
    $populator['xslt'] = theme('file_link', array(
      'file' => file_load($populator['xslt']),
    ));
  }
  $form['#tree'] = TRUE;
  $form['populators'] = array(
    '#type' => 'vertical_tabs',
  );
  $destination = 'public://islandora_xslt_populator_xslts';
  file_prepare_directory($destination);
  $form += array(
    'existing' => array(
      '#type' => 'fieldset',
      '#group' => 'populators',
      '#title' => t('Existing XSLT Populators'),
      'table' => array(
        '#type' => 'tableselect',
        '#header' => array(
          'title' => t('Title'),
          'description' => t('Description'),
          'xslt' => t('XSLT File'),
          'datastream_id' => t('Generated Datastream ID'),
          'machine_name' => t('Machine Name'),
        ),
        '#options' => $populators,
        '#empty' => t('None configured.'),
      ),
      'remove' => array(
        '#type' => 'submit',
        '#value' => t('Remove Selected'),
        '#limit_validation_errors' => array(
          array('existing'),
        ),
        '#validate' => array(
          'islandora_xslt_populator_admin_form_remove_validate',
        ),
        '#submit' => array(
          'islandora_xslt_populator_admin_form_remove_submit',
        ),
      ),
    ),
    'new' => array(
      '#type' => 'fieldset',
      '#title' => t('Add New XSLT Populator'),
      '#group' => 'populators',
      'title' => array(
        '#type' => 'textfield',
        '#title' => t('Title'),
        '#required' => TRUE,
      ),
      'description' => array(
        '#type' => 'textarea',
        '#title' => t('Description'),
        '#required' => TRUE,
      ),
      'xslt' => array(
        '#type' => 'managed_file',
        '#title' => t('XSLT File'),
        '#required' => TRUE,
        '#islandora_plupload_do_not_alter' => TRUE,
        '#upload_location' => $destination,
        '#upload_validators' => array(
          'file_validate_extensions' => array('xsl xslt xml'),
        ),
      ),
      'datastream_id' => array(
        '#type' => 'textfield',
        '#title' => t('Datastream ID'),
        '#description' => t('The datastream identifier into which the result of the XSLT will be stored.'),
        '#required' => TRUE,
      ),
      'machine_name' => array(
        '#type' => 'textfield',
        '#title' => t('Machine Name'),
        '#description' => t('A machine name for use in features. If not provided, the relevent configuration will not be available for export in features.'),
        '#required' => FALSE,
      ),
      'add' => array(
        '#type' => 'submit',
        '#value' => t('Add'),
        '#limit_validation_errors' => array(
          array('new'),
        ),
        '#validate' => array(
          'islandora_xslt_populator_admin_form_add_validate',
        ),
        '#submit' => array(
          'islandora_xslt_populator_admin_form_add_submit',
        ),
      ),
    ),
  );
  return $form;
}

/**
 * Form validation handler; ensure a populator has been selected.
 */
function islandora_xslt_populator_admin_form_remove_validate(&$form, &$form_state) {
  $selected = array_filter($form_state['values']['existing']['table']);
  if (empty($selected)) {
    form_error($form['existing']['table'], t('No populator was selected to remove.'));
  }
}

/**
 * Form submission handler; remove selected populators.
 */
function islandora_xslt_populator_admin_form_remove_submit(&$form, &$form_state) {
  islandora_xslt_populator_remove_populator(array_keys(array_filter($form_state['values']['existing']['table'])));
}

/**
 * Form validation handler; ensure the given datastream ID is valid.
 */
function islandora_xslt_populator_admin_form_add_validate(&$form, &$form_state) {
  module_load_include('inc', 'islandora', 'includes/utilities');
  if (!islandora_is_valid_dsid($form_state['values']['new']['datastream_id'])) {
    form_error($form['new']['datastream_id'], t('The given datastream ID does not appear to be valid.'));
  }
}

/**
 * Form submission handler; add the populator.
 */
function islandora_xslt_populator_admin_form_add_submit(&$form, &$form_state) {
  islandora_xslt_populator_add_populator(
    $form_state['values']['new']['title'],
    $form_state['values']['new']['description'],
    $form_state['values']['new']['xslt'],
    $form_state['values']['new']['datastream_id'],
    $form_state['values']['new']['machine_name']
  );
}
