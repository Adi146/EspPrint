import {
  LitElement,
  html,
  css
} from "https://unpkg.com/lit-element@2.4.0/lit-element.js?module";

class FilesystemCard extends LitElement {
  static get properties() {
    return {
      _hass: {},
      config: {},
      files: {type: Array}
    };
  }

  constructor() {
    super();
    this.config = {};
    this.files = [];
  }

  render() {
    return html`
      <ha-card header="Files">
        <div class="file_list">
          ${this.files.map(file => {
            return html`
              <div class="file_entry">
                <div>
                  ${file.path}
                  <div class="file_buttons">
                    <button @click="${(e) => {
                      const [domain, service] = this.config.print_service.split('.', 2);
                      this._hass.callService(domain, service, {file: file.path});
                    }}">
                      <ha-icon icon="mdi:printer-3d-nozzle"/>
                    </button>
                    <button @click="${(e) => {
                      const [domain, service] = this.config.delete_service.split('.', 2);
                      this._hass.callService(domain, service, {file: file.path});
                    }}">
                      <ha-icon icon="mdi:trash-can"/>
                    </button>
                  </div>
                  <div class="attribute">
                    Size: ${(file.size / (1024 * 1024)).toFixed(2)}MB
                  </div>
                  <div class="attribute">
                    Upload: ${new Date(file.lastWrite * 1000).toLocaleDateString()}
                </div>
              </div>
            `;
          })}
        </div>
      </ha-card>
    `;
  }

  set hass(hass) {
    if (!this._hass) {
      this._hass = hass;
      
      this._hass.connection.subscribeEvents((event) => {
        this.files = JSON.parse(event.data.files);    
      }, this.config.event);

      const [domain, service] = this.config.list_service.split('.', 2);
      this._hass.callService(domain, service, {});
    }
  }

  setConfig(config) {
    if (config.printer_name) {
      this.config.event = "esphome." + config.printer_name + "_files";
      this.config.list_service = "esphome.espprint_" + config.printer_name + "_list_files";
      this.config.print_service = "esphome.espprint_" + config.printer_name + "_print_file";
      this.config.delete_service = "esphome.espprint_" + config.printer_name + "_delete_file";
    }

    if (config.event) {
      this.config.event = config.event;
    }
    if (config.list_service) {
      this.config.list_service = config.list_service;
    }
    if (config.print_service) {
      this.config.print_service = config.print_service;
    }
    if (config.delete_service) {
      this.config.delete_service = config.delete_service;
    }

    this.checkConfig();
  }

  checkConfig() {
    if (!this.config.event) {
      throw new Error("event undefined");
    }
    if (!this.config.list_service) {
      throw new Error("list_service undefined");
    }
    if (!this.config.print_service) {
      throw new Error("print_service undefined");
    }
    if (!this.config.delete_service) {
      throw new Error("delete_service undefined");
    }
  }

  static get styles() {
    return css`
      .file_list {
        overflow: auto;
        height: 400px;
      }
      .file_entry {
        padding: 5px 0px;
        padding-left: 5px;
        padding-right: 5px;
        border-bottom: 1px solid;
      }
      .file_buttons {
        float: right;
      }
      .attribute {
        font-size: small;
      }
    `;
  }
}

customElements.define('filesystem-card', FilesystemCard);