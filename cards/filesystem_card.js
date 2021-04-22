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
      files: {type: Array},
      totalSize: {type: Number},
      usedSize: {type: Number}
    };
  }

  constructor() {
    super();
    this.config = {};
    this.files = [];
    this.totalSize = 0;
    this.usedSize = 0;
  }

  render() {
    return html`
      <ha-card>
        <h1 class="card-header">
          Files
          <div class="fs attribute">
            Storage usage: ${this.readableSize(this.usedSize)} / ${this.readableSize(this.totalSize)}
          </div>
        </h1>

        <div class="file_list">
          ${this.files.map(file => {
            return html`
              <div class="file_entry">
                <div class="file">
                  ${file.path}
                  <div class="attribute">
                    Size: ${this.readableSize(file.size)}
                  </div>
                </div>

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
        this.files = JSON.parse(event.data.files).reverse();
        this.totalSize = event.data.totalSize;
        this.usedSize = event.data.usedSize;
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

  readableSize(size) {
    var i = 0;
    const units = ["Bytes", "kB", "MB", "GB", "TB"];

    while(size > 1024) {
      size /= 1024;
      i++
    }

    return size.toFixed(2) + " " + units[i];
  }

  static get styles() {
    return css`
      .file_list {
        overflow: auto;
        height: 400px;
      }
      .file_entry {
        padding: 5px 16px;
        border-top: 1px solid var(--secondary-background-color);
        overflow: hidden;
      }
      .file {
        float: left;
        width: 70%;
      }
      .file_buttons {
        float: right;
      }
      button {
        background-color: var(--ha-card-background);
        border-radius: 2px;
        padding: 8px;
        border: 1px solid var(--paper-item-icon-color);
        cursor: pointer;
      }
      button:hover, button:focus {
        outline: none;
        filter: brightness(120%);
      }
      ha-icon {
        color: var(--secondary-text-color);
      }
      .attribute {
        font-size: small;
        color: var(--secondary-text-color);
      }
      .fs {
        float: right;
      }
    `;
  }
}

customElements.define('filesystem-card', FilesystemCard);