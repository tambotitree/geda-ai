provider "google" {
  project = "YOUR_PROJECT_ID" # Replace with your actual GCP Project ID
  region  = "us-central1"    # GPUs are currently restricted to specific regions like us-central1
}

resource "google_cloud_run_v2_service" "gpu_api" {
  name     = "geda-ai"
  location = "us-central1"

  template {
    containers {
      # This points to the image built from your Dockerfile
      image = "gcr.io/YOUR_PROJECT_ID/geda-api:latest"

      resources {
        limits = {
          cpu    = "4"      # High-performance APIs with GPUs require more CPU
          memory = "32Gi"   # Increase system RAM to support MoE offloading
          "nvidia.com/gpu" = "1"
        }
      }

      ports {
        container_port = 8080
      }
    }

    # This selects the GPU type. 
    # For Cloud Run, 'nvidia-l4' is the standard accelerator.
    node_selector {
      accelerator = "nvidia-l4"
    }

    scaling {
      max_instance_count = 2 # GPUs are expensive; lower the max scale for safety
    }
  }

  traffic {
    type    = "TRAFFIC_TARGET_ALLOCATION_TYPE_LATEST"
    percent = 100
  }
}

# Note: Removing the public_access resource ensures the API is secured by Google IAM.
# Callers will now need to provide a Google ID Token in the Authorization header.
# resource "google_cloud_run_v2_service_iam_member" "public_access" {
#   name   = google_cloud_run_v2_service.gpu_api.name
#   location = google_cloud_run_v2_service.gpu_api.location
#   role   = "roles/run.invoker"
#   member = "allUsers"
# }
